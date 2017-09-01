#pragma once

#include <random>
#include "MCTS/board/Board.h"
#include "MCTS/policy/RandomByRand.h"
#include "NeuralNetwork.h"

namespace mcts
{
	namespace policy
	{
		namespace simulation
		{
			class ChoiceGetter
			{
			public:
				ChoiceGetter(int choices) : choices_(choices) {}

				size_t Size() const { return (size_t)choices_; }

				int Get(size_t idx) const { return (int)idx; }

				template <typename Functor>
				void ForEachChoice(Functor&& functor) const {
					for (int i = 0; i < choices_; ++i) {
						if (!functor(i)) return;
					}
				}

			private:
				int choices_;
			};

			class RandomPolicy
			{
			public:
				static constexpr bool kEnableCutoff = false;

				RandomPolicy(state::PlayerSide side, std::mt19937 & rand) :
					rand_(rand)
				{
				}

				Result GetCutoffResult(board::Board const& board) {
					assert(false);
					return Result::kResultNotDetermined;
				}

				int GetChoice(
					board::Board const& board,
					FlowControl::FlowContext & flow_context,
					board::BoardActionAnalyzer & action_analyzer,
					ActionType action_type,
					ChoiceGetter const& choice_getter)
				{
					size_t count = choice_getter.Size();
					assert(count > 0);
					size_t rand_idx = (size_t)(rand_() % count);
					int result = choice_getter.Get(rand_idx);
					assert([&]() {
						int result2 = -1;
						size_t idx = 0;
						choice_getter.ForEachChoice([&](int choice) {
							if (idx == rand_idx) {
								result2 = choice;
								return false;
							}
							++idx;
							return true;
						});
						return result == result2;
					}());
					return result;
				}

			private:
				std::mt19937 & rand_;
			};

			class WeakHeuristicStateValueFunction
			{
			public:
				double GetStateValue(board::Board const& board) {
					return board.ApplyWithPlayerStateView([&](auto const& view) {
						return GetStateValueForSide(board.GetViewSide(), view);
					});
				}

				bool GuessWillWin(board::Board const& board) {
					double v = GetStateValue(board);
					static constexpr double draw_v = 0.0;
					return (v > draw_v);
				}

			private:
				template <state::PlayerSide Side>
				double GetStateValueForSide(state::PlayerSide self_side, FlowControl::PlayerStateView<Side> view) {
					assert(self_side == Side);
					state::PlayerSide opponent_side = state::PlayerIdentifier(self_side).Opposite().GetSide();

					double v = 0.0;

					view.ForEachMinion(self_side, [&](state::Cards::Card const& card, bool attackable) {
						v += 10.0 * GetMinionValue(card);
					});
					view.ForEachMinion(opponent_side, [&](state::Cards::Card const& card, bool attackable) {
						v += -10.0 * GetMinionValue(card);
					});

					v += 3.0 * GetHeroValue(view.GetSelfHero());
					v += -3.0 * GetHeroValue(view.GetOpponentHero());

					return v;
				}

				double GetMinionValue(state::Cards::Card const& card) {
					return 1.0 * card.GetAttack() + 1.5 * card.GetHP();
				}

				double GetHeroValue(state::Cards::Card const& card) {
					double v = card.GetHP() + card.GetArmor();
					if (v >= 15) v = 15;

					return v;
				}
			};

			class NeuralNetworkStateValueFunction
			{
			public:
				NeuralNetworkStateValueFunction()
					: filename_(), net_(), current_player_viewer_()
				{
					filename_ = "simulation_net";
					LoadNetwork();
				}

				void LoadNetwork() {
					net_.InitializePredict(filename_);
				}

				bool GuessWillWin(board::Board const& board) {
					double v = GetStateValue(board);
					static constexpr double draw_v = 0.0;
					return (v > draw_v);
				}

				double GetStateValue(board::Board const& board) {
					return GetStateValue(board.RevealHiddenInformationForSimulation(), board.GetViewSide());
				}

			private:
				class StateDataBridge : public NeuralNetworkWrapper::IInputGetter
				{
				public:
					StateDataBridge() : state_(nullptr), attackable_indics_(),
						playable_cards_(), hero_power_playable_()
					{}

					StateDataBridge(StateDataBridge const&) = delete;
					StateDataBridge & operator=(StateDataBridge const&) = delete;

					void Reset(state::State const& state) {
						state_ = &state;

						FlowControl::ValidActionGetter valid_action(*state_);

						attackable_indics_.clear();
						valid_action.ForEachAttacker([this](int encoded_idx) {
							attackable_indics_.push_back(encoded_idx);
							return true;
						});

						playable_cards_.clear();
						valid_action.ForEachPlayableCard([&](size_t idx) {
							playable_cards_.push_back((int)idx);
							return true;
						});

						hero_power_playable_ = valid_action.CanUseHeroPower();
					}

					double GetField(
						NeuralNetworkWrapper::FieldSide field_side,
						NeuralNetworkWrapper::FieldType field_type,
						int arg1 = 0) override final
					{
						if (field_side == NeuralNetworkWrapper::kCurrent) {
							return GetSideField(field_type, arg1, state_->GetCurrentPlayer());
						}
						else if (field_side == NeuralNetworkWrapper::kOpponent) {
							return GetSideField(field_type, arg1, state_->GetOppositePlayer());
						}
						throw std::runtime_error("invalid side");
					}

				private:
					double GetSideField(NeuralNetworkWrapper::FieldType field_type, int arg1, state::board::Player const& player) {
						switch (field_type) {
						case NeuralNetworkWrapper::kResourceCurrent:
						case NeuralNetworkWrapper::kResourceTotal:
						case NeuralNetworkWrapper::kResourceOverload:
						case NeuralNetworkWrapper::kResourceOverloadNext:
							return GetResourceField(field_type, arg1, player.GetResource());

						case NeuralNetworkWrapper::kHeroHP:
						case NeuralNetworkWrapper::kHeroArmor:
							return GetHeroField(field_type, arg1, state_->GetCard(player.GetHeroRef()));

						case NeuralNetworkWrapper::kMinionCount:
						case NeuralNetworkWrapper::kMinionHP:
						case NeuralNetworkWrapper::kMinionMaxHP:
						case NeuralNetworkWrapper::kMinionAttack:
						case NeuralNetworkWrapper::kMinionAttackable:
						case NeuralNetworkWrapper::kMinionTaunt:
						case NeuralNetworkWrapper::kMinionShield:
						case NeuralNetworkWrapper::kMinionStealth:
							return GetMinionsField(field_type, arg1, player.minions_);

						case NeuralNetworkWrapper::kHandCount:
						case NeuralNetworkWrapper::kHandPlayable:
						case NeuralNetworkWrapper::kHandCost:
							return GetHandField(field_type, arg1, player.hand_);

						case NeuralNetworkWrapper::kHeroPowerPlayable:
							return GetHeroPowerField(field_type, arg1);

						default:
							throw std::runtime_error("unknown field type");
						}
					}

					double GetResourceField(NeuralNetworkWrapper::FieldType field_type, int arg1, state::board::PlayerResource const& resource) {
						switch (field_type) {
						case NeuralNetworkWrapper::kResourceCurrent:
							return resource.GetCurrent();
						case NeuralNetworkWrapper::kResourceTotal:
							return resource.GetTotal();
						case NeuralNetworkWrapper::kResourceOverload:
							return resource.GetCurrentOverloaded();
						case NeuralNetworkWrapper::kResourceOverloadNext:
							return resource.GetNextOverload();
						default:
							throw std::runtime_error("unknown field type");
						}
					}

					double GetHeroField(NeuralNetworkWrapper::FieldType field_type, int arg1, state::Cards::Card const& hero) {
						switch (field_type) {
						case NeuralNetworkWrapper::kHeroHP:
							return hero.GetHP();
						case NeuralNetworkWrapper::kHeroArmor:
							return hero.GetArmor();
						default:
							throw std::runtime_error("unknown field type");
						}
					}

					double GetMinionsField(NeuralNetworkWrapper::FieldType field_type, int minion_idx, state::board::Minions const& minions) {
						switch (field_type) {
						case NeuralNetworkWrapper::kMinionCount:
							return (double)minions.Size();
						case NeuralNetworkWrapper::kMinionHP:
						case NeuralNetworkWrapper::kMinionMaxHP:
						case NeuralNetworkWrapper::kMinionAttack:
						case NeuralNetworkWrapper::kMinionAttackable:
						case NeuralNetworkWrapper::kMinionTaunt:
						case NeuralNetworkWrapper::kMinionShield:
						case NeuralNetworkWrapper::kMinionStealth:
							return GetMinionField(field_type, minion_idx, state_->GetCard(minions.Get(minion_idx)));
						default:
							throw std::runtime_error("unknown field type");
						}
					}

					double GetMinionField(NeuralNetworkWrapper::FieldType field_type, int minion_idx, state::Cards::Card const& minion) {
						switch (field_type) {
						case NeuralNetworkWrapper::kMinionHP:
							return minion.GetHP();
						case NeuralNetworkWrapper::kMinionMaxHP:
							return minion.GetMaxHP();
						case NeuralNetworkWrapper::kMinionAttack:
							return minion.GetAttack();
						case NeuralNetworkWrapper::kMinionAttackable:
							return (std::find(attackable_indics_.begin(), attackable_indics_.end(), minion_idx) != attackable_indics_.end());
						case NeuralNetworkWrapper::kMinionTaunt:
							return minion.HasTaunt();
						case NeuralNetworkWrapper::kMinionShield:
							return minion.HasShield();
						case NeuralNetworkWrapper::kMinionStealth:
							return minion.HasStealth();
						default:
							throw std::runtime_error("unknown field type");
						}
					}

					double GetHandField(NeuralNetworkWrapper::FieldType field_type, int hand_idx, state::board::Hand const& hand) {
						switch (field_type) {
						case NeuralNetworkWrapper::kHandCount:
							return (double)hand.Size();
						case NeuralNetworkWrapper::kHandPlayable:
						case NeuralNetworkWrapper::kHandCost:
							return GetHandCardField(field_type, hand_idx, state_->GetCard(hand.Get(hand_idx)));
						default:
							throw std::runtime_error("unknown field type");
						}
					}

					double GetHandCardField(NeuralNetworkWrapper::FieldType field_type, int hand_idx, state::Cards::Card const& card) {
						switch (field_type) {
						case NeuralNetworkWrapper::kHandPlayable:
							return (std::find(playable_cards_.begin(), playable_cards_.end(), hand_idx) != playable_cards_.end());
						case NeuralNetworkWrapper::kHandCost:
							return card.GetCost();
						default:
							throw std::runtime_error("unknown field type");
						}
					}

					double GetHeroPowerField(NeuralNetworkWrapper::FieldType field_type, int arg1) {
						switch (field_type) {
						case NeuralNetworkWrapper::kHeroPowerPlayable:
							return hero_power_playable_;
						default:
							throw std::runtime_error("unknown field type");
						}
					}

				private:
					state::State const* state_;
					std::vector<int> attackable_indics_;
					std::vector<int> playable_cards_;
					bool hero_power_playable_;
				};

				double GetStateValue(state::State const& state, state::PlayerSide view_side) {
					state::PlayerSide current_side = state.GetCurrentPlayerId().GetSide();
					current_player_viewer_.Reset(state);
					double score_from_current_player = net_.Predict(&current_player_viewer_);

					if (view_side != current_side) {
						return -score_from_current_player;
					}
					else {
						return score_from_current_player;
					}
				}

			private:
				std::string filename_;
				NeuralNetworkWrapper net_;
				StateDataBridge current_player_viewer_;
			};

			class HeuristicPolicy
			{
			public:
				// Simulation cutoff:
				// For each simulation choice, a small probability is defined so the simulation ends here,
				//    and the game result is defined by a heuristic state-value function.
				// Assume the cutoff probability is p,
				//    The expected number of simulation runs is 1/p.
				//    So, if the expected number of runs is N, the probability p = 1.0 / N
				static constexpr bool kEnableCutoff = true;
				static constexpr double kCutoffExpectedRuns = 10;
				static constexpr double kCutoffProbability = 1.0 / kCutoffExpectedRuns;

				Result GetCutoffResult(board::Board const& board) {
					std::uniform_real_distribution<double> rand_gen(0.0, 1.0);
					double v = rand_gen(rand_);
					if (v >= kCutoffProbability) {
						return Result::kResultNotDetermined;
					}

					bool self_win = state_value_func_.GuessWillWin(board);
					if (self_win) {
						if (board.GetViewSide() == state::kPlayerFirst) return Result::kResultFirstPlayerWin;
						else return Result::kResultSecondPlayerWin;
					}
					else {
						if (board.GetViewSide() == state::kPlayerSecond) return Result::kResultSecondPlayerWin;
						else return Result::kResultFirstPlayerWin;
					}
				}

			public:
				HeuristicPolicy(state::PlayerSide side, std::mt19937 & rand) :
					rand_(rand),
					decision_(),
					state_value_func_(),
					copy_board_(side)
				{
				}

				int GetChoice(
					board::Board const& board,
					FlowControl::FlowContext & flow_context,
					board::BoardActionAnalyzer & action_analyzer,
					ActionType action_type,
					ChoiceGetter const& choice_getter)
				{
					if (action_type == ActionType::kMainAction) {
						StartNewAction(board, flow_context, action_analyzer);

						return GetChoiceForMainAction(
							board, action_analyzer, choice_getter);
					}

					// TODO: use value network to enhance simulation
					size_t count = choice_getter.Size();
					assert(count > 0);
					size_t idx = 0;
					size_t rand_idx = (size_t)(std::rand() % count);
					int result = -1;
					choice_getter.ForEachChoice([&](int choice) {
						if (idx == rand_idx) {
							result = choice;
							return false;
						}
						++idx;
						return true;
					});
					assert([&]() {
						int result2 = choice_getter.Get(rand_idx);
						return result == result2;
					}());
					return result;
				}

			private:
				void StartNewAction(
					board::Board const& board,
					FlowControl::FlowContext & flow_context,
					board::BoardActionAnalyzer & action_analyzer)
				{
					DFSBestStateValue(board, flow_context, action_analyzer);
					assert(!decision_.empty());
				}

				void DFSBestStateValue(
					board::Board const& board,
					FlowControl::FlowContext & flow_context,
					board::BoardActionAnalyzer & action_analyzer)
				{
					class RandomPolicy : public mcts::board::IRandomGenerator {
					public:
						RandomPolicy(int seed) : rand_(seed) {}
						int Get(int exclusive_max) final { return rand_.GetRandom(exclusive_max); }

					private:
						FastRandom rand_;
					};

					struct DFSItem {
						size_t choice_;
						size_t total_;

						DFSItem(size_t choice, size_t total) : choice_(choice), total_(total) {}
					};

					class UserChoicePolicy : public mcts::board::IActionParameterGetter {
					public:
						UserChoicePolicy(std::vector<DFSItem> & dfs,
							std::vector<DFSItem>::iterator & dfs_it,
							int seed) :
							dfs_(dfs), dfs_it_(dfs_it), rand_(seed)
						{}

						int GetNumber(ActionType::Types action_type, board::ActionChoices const& action_choices) {

							int total = action_choices.Size();

							assert(action_type != ActionType::kRandom);
							
							if (action_type == ActionType::kChooseMinionPutLocation) {
								assert(total >= 1);
								int idx = rand_.GetRandom(total);
								return action_choices.Get(idx);
							}

							if (dfs_it_ == dfs_.end()) {
								assert(total >= 1);
								dfs_.emplace_back(0, (size_t)total);
								dfs_it_ = dfs_.end();
								return action_choices.Get(0);
							}
							
							assert(dfs_it_->total_ == (size_t)total);
							size_t idx = dfs_it_->choice_;
							assert((int)idx < total);
							++dfs_it_;
							return action_choices.Get(idx);
						}

					private:
						std::vector<DFSItem> & dfs_;
						std::vector<DFSItem>::iterator & dfs_it_;
						FastRandom rand_;
					};

					std::vector<DFSItem> dfs;
					std::vector<DFSItem>::iterator dfs_it = dfs.begin();

					auto step_next_dfs = [&]() {
						while (!dfs.empty()) {
							if ((dfs.back().choice_ + 1) < dfs.back().total_) {
								break;
							}
							dfs.pop_back();
						}

						if (dfs.empty()) return false; // all done

						++dfs.back().choice_;
						return true;
					};

					// Need to fix a random sequence for a particular run
					// Since, some callbacks might depend on a random
					// For example, choose one card from randomly-chosen three cards
					RandomPolicy cb_random(rand_());
					UserChoicePolicy cb_user_choice(dfs, dfs_it, rand_());

					double best_value = -std::numeric_limits<double>::infinity();
					action_analyzer.ForEachMainOp([&](size_t idx, board::BoardActionAnalyzer::OpType main_op) {
						assert(board.GetViewSide() == copy_board_.GetBoard().GetViewSide());

						while (true) {
							copy_board_.FillWithBase(board);

							dfs_it = dfs.begin();
							auto result = copy_board_.GetBoard().ApplyAction(
								(int)idx,
								action_analyzer, flow_context,
								cb_random,
								cb_user_choice);

							if (result != Result::kResultInvalid) {
								double value = -std::numeric_limits<double>::infinity();
								int self_win = copy_board_.GetBoard().IsSelfWin(result);
								if (self_win == 1) {
									value = std::numeric_limits<double>::infinity();
								}
								else if (self_win == -1) {
									value = -std::numeric_limits<double>::infinity();
								}
								else {
									state_value_func_.GetStateValue(copy_board_.GetBoard());
								}

								if (decision_.empty() || value > best_value) {
									best_value = value;

									decision_.clear();
									decision_.push_back((int)idx);
									for (auto const& item : dfs) {
										decision_.push_back((int)item.choice_);
									}
								}
							}

							if (!step_next_dfs()) break; // all done
						}
						return true;
					});
				}

				int GetChoiceForMainAction(
					board::Board const& board,
					board::BoardActionAnalyzer const& action_analyzer,
					ChoiceGetter const& choice_getter)
				{
					// TODO: maybe we don't need to rule out end-turn manually
					// it will be naturally ruled out, since its state-value should be lower

					// choose end-turn only when it is the only option
					size_t count = choice_getter.Size();
					assert(count > 0);
					if (count == 1) {
						// should be end-turn
						assert(action_analyzer.GetMainOpType((size_t)0) == board::BoardActionAnalyzer::kEndTurn);
						return 0;
					}

					// rule out the end-turn action
					assert(action_analyzer.GetMainOpType(count - 1) == board::BoardActionAnalyzer::kEndTurn);
					--count;
					assert(count > 0);

					// otherwise, choose randomly
					size_t rand_idx = (size_t)(std::rand() % count);
					return choice_getter.Get(rand_idx);
				}

			private:
				std::mt19937 & rand_;

				std::vector<int> decision_;
				NeuralNetworkStateValueFunction state_value_func_;
				board::CopiedBoard copy_board_;
			};
		}
	}
}
