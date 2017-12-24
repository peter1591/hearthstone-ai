#pragma once

#include <random>
#include "engine/view/Board.h"
#include "MCTS/policy/RandomByRand.h"
#include "neural_net/NeuralNetwork.h"

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

			class RandomPlayouts
			{
			public:
				static constexpr bool kEnableCutoff = false;

				RandomPlayouts(state::PlayerSide side, std::mt19937 & rand) :
					rand_(rand)
				{
				}

				engine::Result GetCutoffResult(engine::view::Board const& board) {
					assert(false);
					return engine::kResultNotDetermined;
				}

				int GetChoice(
					engine::view::Board const& board,
					engine::ValidActionAnalyzer const& action_analyzer,
					engine::ActionType action_type,
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
			private:
				static constexpr double kMaxMinionValue = 12.5;
				double GetMinionValue(state::Cards::Card const& card) {
					return 1.0 * card.GetAttack() + 1.5 * card.GetHP();
				}

				static constexpr double kMaxHeroValue = 30.0;
				double GetHeroValue(state::Cards::Card const& card) {
					double v = card.GetHP() + card.GetArmor();
					if (v >= 30) v = 30;

					return v;
				}

				//static constexpr double kMaxSideValue = 3.0 * kMaxHeroValue + 10.0 * kMaxMinionValue * 7;
				static constexpr double kMaxSideValue = kMaxHeroValue;
				template <state::PlayerSide Side>
				double GetStateValueForSide(state::PlayerSide self_side, engine::view::BoardRefView<Side> view) {
					assert(self_side == Side);
					//state::PlayerSide opponent_side = state::PlayerIdentifier(self_side).Opposite().GetSide();

					double v = 0.0;

					//view.ForEachMinion(self_side, [&](state::Cards::Card const& card, bool attackable) {
					//	v += 10.0 * GetMinionValue(card);
					//	return true;
					//});
					//view.ForEachMinion(opponent_side, [&](state::Cards::Card const& card, bool attackable) {
					//	v += -10.0 * GetMinionValue(card);
					//	return true;
					//});

					//v += 3.0 * GetHeroValue(view.GetSelfHero());
					//v += -3.0 * GetHeroValue(view.GetOpponentHero());
					v += 30.0 - GetHeroValue(view.GetOpponentHero());
					
					v = v / kMaxSideValue;

					return v;
				}

			public:
				double GetStateValue(state::State const& game_state) {
					engine::view::BoardRefView<state::kPlayerFirst> view(game_state);
					return GetStateValueForSide(state::kPlayerFirst, view);
				}

				// State value is in range [-1, 1]
				// If 100% wins, score = 1.0
				// If 50% wins, 50%loss, score = 0.0
				// If 100% loss, score = -1.0
				double GetStateValue(engine::view::Board const& board) {
					// TODO: should always return state value for first player?
					return board.ApplyWithPlayerStateView([&](auto const& view) {
						return GetStateValueForSide(board.GetViewSide(), view);
					});
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

				// State value is in range [-1, 1]
				// If first player is 100% wins, score = 1.0
				// If first player is 50% wins, 50%loss, score = 0.0
				// If first player is 100% loss, score = -1.0
				double GetStateValue(engine::view::Board const& board) {
					return GetStateValue(board.RevealHiddenInformationForSimulation());
				}

				double GetStateValue(state::State const& state) {
					current_player_viewer_.Reset(state);

					double score = net_.Predict(&current_player_viewer_);

					if (!state.GetCurrentPlayerId().IsFirst()) {
						score = -score;
					}

					if (score > 1.0) score = 1.0;
					if (score < -1.0) score = -1.0;

					return score;
				}

			private:
				class StateDataBridge : public neural_net::NeuralNetworkWrapper::IInputGetter
				{
				public:
					StateDataBridge() : state_(nullptr), attackable_indics_(),
						playable_cards_(), hero_power_playable_()
					{}

					StateDataBridge(StateDataBridge const&) = delete;
					StateDataBridge & operator=(StateDataBridge const&) = delete;

					void Reset(state::State const& state) {
						state_ = &state;

						engine::FlowControl::ValidActionGetter valid_action(*state_);

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
						neural_net::NeuralNetworkWrapper::FieldSide field_side,
						neural_net::NeuralNetworkWrapper::FieldType field_type,
						int arg1 = 0) override final
					{
						if (field_side == neural_net::NeuralNetworkWrapper::kCurrent) {
							return GetSideField(field_type, arg1, state_->GetCurrentPlayer());
						}
						else if (field_side == neural_net::NeuralNetworkWrapper::kOpponent) {
							return GetSideField(field_type, arg1, state_->GetOppositePlayer());
						}
						throw std::runtime_error("invalid side");
					}

				private:
					double GetSideField(neural_net::NeuralNetworkWrapper::FieldType field_type, int arg1, state::board::Player const& player) {
						switch (field_type) {
						case neural_net::NeuralNetworkWrapper::kResourceCurrent:
						case neural_net::NeuralNetworkWrapper::kResourceTotal:
						case neural_net::NeuralNetworkWrapper::kResourceOverload:
						case neural_net::NeuralNetworkWrapper::kResourceOverloadNext:
							return GetResourceField(field_type, arg1, player.GetResource());

						case neural_net::NeuralNetworkWrapper::kHeroHP:
						case neural_net::NeuralNetworkWrapper::kHeroArmor:
							return GetHeroField(field_type, arg1, state_->GetCard(player.GetHeroRef()));

						case neural_net::NeuralNetworkWrapper::kMinionCount:
						case neural_net::NeuralNetworkWrapper::kMinionHP:
						case neural_net::NeuralNetworkWrapper::kMinionMaxHP:
						case neural_net::NeuralNetworkWrapper::kMinionAttack:
						case neural_net::NeuralNetworkWrapper::kMinionAttackable:
						case neural_net::NeuralNetworkWrapper::kMinionTaunt:
						case neural_net::NeuralNetworkWrapper::kMinionShield:
						case neural_net::NeuralNetworkWrapper::kMinionStealth:
							return GetMinionsField(field_type, arg1, player.minions_);

						case neural_net::NeuralNetworkWrapper::kHandCount:
						case neural_net::NeuralNetworkWrapper::kHandPlayable:
						case neural_net::NeuralNetworkWrapper::kHandCost:
							return GetHandField(field_type, arg1, player.hand_);

						case neural_net::NeuralNetworkWrapper::kHeroPowerPlayable:
							return GetHeroPowerField(field_type, arg1);

						default:
							throw std::runtime_error("unknown field type");
						}
					}

					double GetResourceField(neural_net::NeuralNetworkWrapper::FieldType field_type, int arg1, state::board::PlayerResource const& resource) {
						switch (field_type) {
						case neural_net::NeuralNetworkWrapper::kResourceCurrent:
							return resource.GetCurrent();
						case neural_net::NeuralNetworkWrapper::kResourceTotal:
							return resource.GetTotal();
						case neural_net::NeuralNetworkWrapper::kResourceOverload:
							return resource.GetCurrentOverloaded();
						case neural_net::NeuralNetworkWrapper::kResourceOverloadNext:
							return resource.GetNextOverload();
						default:
							throw std::runtime_error("unknown field type");
						}
					}

					double GetHeroField(neural_net::NeuralNetworkWrapper::FieldType field_type, int arg1, state::Cards::Card const& hero) {
						switch (field_type) {
						case neural_net::NeuralNetworkWrapper::kHeroHP:
							return hero.GetHP();
						case neural_net::NeuralNetworkWrapper::kHeroArmor:
							return hero.GetArmor();
						default:
							throw std::runtime_error("unknown field type");
						}
					}

					double GetMinionsField(neural_net::NeuralNetworkWrapper::FieldType field_type, int minion_idx, state::board::Minions const& minions) {
						switch (field_type) {
						case neural_net::NeuralNetworkWrapper::kMinionCount:
							return (double)minions.Size();
						case neural_net::NeuralNetworkWrapper::kMinionHP:
						case neural_net::NeuralNetworkWrapper::kMinionMaxHP:
						case neural_net::NeuralNetworkWrapper::kMinionAttack:
						case neural_net::NeuralNetworkWrapper::kMinionAttackable:
						case neural_net::NeuralNetworkWrapper::kMinionTaunt:
						case neural_net::NeuralNetworkWrapper::kMinionShield:
						case neural_net::NeuralNetworkWrapper::kMinionStealth:
							return GetMinionField(field_type, minion_idx, state_->GetCard(minions.Get(minion_idx)));
						default:
							throw std::runtime_error("unknown field type");
						}
					}

					double GetMinionField(neural_net::NeuralNetworkWrapper::FieldType field_type, int minion_idx, state::Cards::Card const& minion) {
						switch (field_type) {
						case neural_net::NeuralNetworkWrapper::kMinionHP:
							return minion.GetHP();
						case neural_net::NeuralNetworkWrapper::kMinionMaxHP:
							return minion.GetMaxHP();
						case neural_net::NeuralNetworkWrapper::kMinionAttack:
							return minion.GetAttack();
						case neural_net::NeuralNetworkWrapper::kMinionAttackable:
							return (std::find(attackable_indics_.begin(), attackable_indics_.end(), minion_idx) != attackable_indics_.end());
						case neural_net::NeuralNetworkWrapper::kMinionTaunt:
							return minion.HasTaunt();
						case neural_net::NeuralNetworkWrapper::kMinionShield:
							return minion.HasShield();
						case neural_net::NeuralNetworkWrapper::kMinionStealth:
							return minion.HasStealth();
						default:
							throw std::runtime_error("unknown field type");
						}
					}

					double GetHandField(neural_net::NeuralNetworkWrapper::FieldType field_type, int hand_idx, state::board::Hand const& hand) {
						switch (field_type) {
						case neural_net::NeuralNetworkWrapper::kHandCount:
							return (double)hand.Size();
						case neural_net::NeuralNetworkWrapper::kHandPlayable:
						case neural_net::NeuralNetworkWrapper::kHandCost:
							return GetHandCardField(field_type, hand_idx, state_->GetCard(hand.Get(hand_idx)));
						default:
							throw std::runtime_error("unknown field type");
						}
					}

					double GetHandCardField(neural_net::NeuralNetworkWrapper::FieldType field_type, int hand_idx, state::Cards::Card const& card) {
						switch (field_type) {
						case neural_net::NeuralNetworkWrapper::kHandPlayable:
							return (std::find(playable_cards_.begin(), playable_cards_.end(), hand_idx) != playable_cards_.end());
						case neural_net::NeuralNetworkWrapper::kHandCost:
							return card.GetCost();
						default:
							throw std::runtime_error("unknown field type");
						}
					}

					double GetHeroPowerField(neural_net::NeuralNetworkWrapper::FieldType field_type, int arg1) {
						switch (field_type) {
						case neural_net::NeuralNetworkWrapper::kHeroPowerPlayable:
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

			private:
				std::string filename_;
				neural_net::NeuralNetworkWrapper net_;
				StateDataBridge current_player_viewer_;
			};

			class RandomPlayoutWithHeuristicEarlyCutoffPolicy
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

				engine::Result GetCutoffResult(engine::view::Board const& board) {
					std::uniform_real_distribution<double> rand_gen(0.0, 1.0);
					double v = rand_gen(rand_);
					if (v >= kCutoffProbability) {
						return engine::kResultNotDetermined;
					}

					double score = state_value_func_.GetStateValue(board);
					if (score > 0.0) return engine::kResultFirstPlayerWin;
					else if (score == 0.0) return engine::kResultDraw;
					else return engine::kResultSecondPlayerWin;
				}

			public:
				RandomPlayoutWithHeuristicEarlyCutoffPolicy(state::PlayerSide side, std::mt19937 & rand) :
					rand_(rand),
					state_value_func_()
				{
				}

				int GetChoice(
					engine::view::Board const& board,
					engine::ValidActionAnalyzer const& action_analyzer,
					engine::ActionType action_type,
					ChoiceGetter const& choice_getter)
				{
					size_t count = choice_getter.Size();
					assert(count > 0);
					size_t rand_idx = (size_t)(rand_() % count);
					return choice_getter.Get(rand_idx);
				}

			private:
				std::mt19937 & rand_;
				NeuralNetworkStateValueFunction state_value_func_;
			};

			class HardCodedPlayoutWithHeuristicEarlyCutoffPolicy
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

				engine::Result GetCutoffResult(engine::view::Board const& board) {
					std::uniform_real_distribution<double> rand_gen(0.0, 1.0);
					double v = rand_gen(rand_);
					if (v >= kCutoffProbability) {
						return engine::kResultNotDetermined;
					}
					
					double score = state_value_func_.GetStateValue(board);
					if (score > 0.0) return engine::kResultFirstPlayerWin;
					else if (score == 0.0) return engine::kResultDraw;
					else return engine::kResultSecondPlayerWin;
				}

			public:
				HardCodedPlayoutWithHeuristicEarlyCutoffPolicy(state::PlayerSide side, std::mt19937 & rand) :
					rand_(rand),
					state_value_func_()
				{
				}

				int GetChoice(
					engine::view::Board const& board,
					engine::ValidActionAnalyzer const& action_analyzer,
					engine::ActionType action_type,
					ChoiceGetter const& choice_getter)
				{
					if (action_type != engine::ActionType::kMainAction) {
						size_t count = choice_getter.Size();
						assert(count > 0);
						size_t rand_idx = (size_t)(rand_() % count);
						return choice_getter.Get(rand_idx);
					}
					
					// choose end-turn only when it is the only option
					size_t count = choice_getter.Size();
					assert(count > 0);
					if (count == 1) {
						// should be end-turn
						assert(action_analyzer.GetMainOpType((size_t)0) == engine::MainOpType::kMainOpEndTurn);
						return 0;
					}

					// rule out the end-turn action
					assert(action_analyzer.GetMainOpType(count - 1) == engine::MainOpType::kMainOpEndTurn);
					--count;
					assert(count > 0);

					// otherwise, choose randomly
					size_t rand_idx = (size_t)(rand_() % count);
					return choice_getter.Get(rand_idx);
				}

			private:
				std::mt19937 & rand_;
				NeuralNetworkStateValueFunction state_value_func_;
			};

			class RandomPlayoutWithHardCodedRules
			{
			public:
				static constexpr bool kEnableCutoff = false;

				engine::Result GetCutoffResult(engine::view::Board const& board) {
					return engine::kResultNotDetermined;
				}

			public:
				RandomPlayoutWithHardCodedRules(state::PlayerSide side, std::mt19937 & rand) :
					rand_(rand)
				{
				}

				int GetChoice(
					engine::view::Board const& board,
					engine::ValidActionAnalyzer const& action_analyzer,
					engine::ActionType action_type,
					ChoiceGetter const& choice_getter)
				{
					if (action_type != engine::ActionType::kMainAction) {
						size_t count = choice_getter.Size();
						assert(count > 0);
						size_t rand_idx = (size_t)(rand_() % count);
						return choice_getter.Get(rand_idx);
					}

					// choose end-turn only when it is the only option
					size_t count = choice_getter.Size();
					assert(count > 0);
					if (count == 1) {
						// should be end-turn
						assert(action_analyzer.GetMainOpType((size_t)0) == engine::MainOpType::kMainOpEndTurn);
						return 0;
					}

					// rule out the end-turn action
					assert(action_analyzer.GetMainOpType(count - 1) == engine::MainOpType::kMainOpEndTurn);
					--count;
					assert(count > 0);

					// otherwise, choose randomly
					size_t rand_idx = (size_t)(rand_() % count);
					return choice_getter.Get(rand_idx);
				}

			private:
				std::mt19937 & rand_;
			};

			class HeuristicPlayoutWithHeuristicEarlyCutoffPolicy
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
				static constexpr bool kRandomlyPutMinions = true;

				engine::Result GetCutoffResult(engine::view::Board const& board) {
					std::uniform_real_distribution<double> rand_gen(0.0, 1.0);
					double v = rand_gen(rand_);
					if (v >= kCutoffProbability) {
						return engine::kResultNotDetermined;
					}

					double score = state_value_func_.GetStateValue(board);
					if (score > 0.0) return engine::kResultFirstPlayerWin;
					else if (score == 0.0) return engine::kResultDraw;
					else return engine::kResultSecondPlayerWin;
				}

			public:
				HeuristicPlayoutWithHeuristicEarlyCutoffPolicy(state::PlayerSide side, std::mt19937 & rand) :
					rand_(rand),
					decision_(), decision_idx_(0),
					state_value_func_()
				{
				}

				int GetChoice(
					engine::view::Board const& board,
					engine::ValidActionAnalyzer const& action_analyzer,
					engine::ActionType action_type,
					ChoiceGetter const& choice_getter)
				{
					if (action_type == engine::ActionType::kMainAction) {
						StartNewAction(board, action_analyzer);
					}

					if constexpr (kRandomlyPutMinions) {
						return GetChoiceRandomly(board, action_analyzer, choice_getter);
					}
					else {
						return GetChoiceForMainAction(board, action_analyzer, choice_getter);
					}
				}

			private:
				void StartNewAction(
					engine::view::Board const& board,
					engine::ValidActionAnalyzer const& action_analyzer)
				{
					decision_idx_ = 0;
					DFSBestStateValue(board, action_analyzer);
				}

				void DFSBestStateValue(
					engine::view::Board const& board,
					engine::ValidActionAnalyzer const& action_analyzer)
				{
					struct DFSItem {
						size_t choice_;
						size_t total_;

						DFSItem(size_t choice, size_t total) : choice_(choice), total_(total) {}
					};

					class UserChoicePolicy : public engine::IActionParameterGetter {
					public:
						UserChoicePolicy(std::vector<DFSItem> & dfs,
							std::vector<DFSItem>::iterator & dfs_it,
							int seed) :
							dfs_(dfs), dfs_it_(dfs_it), rand_(seed), main_op_idx_(-1)
						{}

						// TODO: can we remove this? need special care on main op?
						void SetMainOpIndex(int main_op_idx) { main_op_idx_ = main_op_idx; }

						int GetNumber(engine::ActionType::Types action_type, engine::ActionChoices const& action_choices) final {
							if (action_type == engine::ActionType::kMainAction) {
								return main_op_idx_;
							}

							int total = action_choices.Size();

							assert(total >= 1);
							if (total == 1) return 0;

							if (action_type == engine::ActionType::kRandom) {
								return rand_.GetRandom(total);
							}

							assert(action_type != engine::ActionType::kRandom);

							if constexpr (kRandomlyPutMinions) {
								if (action_type == engine::ActionType::kChooseMinionPutLocation) {
									assert(total >= 1);
									int idx = rand_.GetRandom(total);
									return action_choices.Get(idx);
								}
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
						int main_op_idx_;
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
					UserChoicePolicy cb_user_choice(dfs, dfs_it, rand_());
					cb_user_choice.Initialize(board.GetCurrentPlayerStateRefView().GetValidActionGetter());

					double best_value = -std::numeric_limits<double>::infinity();
					action_analyzer.ForEachMainOp([&](size_t main_op_idx, engine::MainOpType main_op) {
						cb_user_choice.SetMainOpIndex((int)main_op_idx);

						while (true) {
							engine::Game copied_game;
							engine::view::Board copy_board(copied_game, board.GetViewSide());
							copy_board.RefCopyFrom(board);

							dfs_it = dfs.begin();
							auto result = copy_board.ApplyAction(cb_user_choice);

							if (result != engine::kResultInvalid) {
								double value = -std::numeric_limits<double>::infinity();
								if (result == engine::kResultFirstPlayerWin) {
									value = std::numeric_limits<double>::infinity();
								}
								else if (result == engine::kResultSecondPlayerWin) {
									value = -std::numeric_limits<double>::infinity();
								}
								else if (result == engine::kResultDraw) {
									value = 0.0;
								}
								else {
									assert(result == engine::kResultNotDetermined);
									value = state_value_func_.GetStateValue(copy_board);
								}

								if (decision_.empty() || value > best_value) {
									best_value = value;

									decision_.clear();
									decision_.push_back((int)main_op_idx);
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
					engine::view::Board const& board,
					engine::ValidActionAnalyzer const& action_analyzer,
					ChoiceGetter const& choice_getter)
				{
					if (decision_idx_ < decision_.size()) {
						return decision_[decision_idx_++];
					}

					// TODO: We fix the random when running dfs search,
					// and in concept, a random might change the best decision.
					// For example, a choose-one with randomly-chosen three cards,
					// we definitely need to re-run the DFS after the random cards are shown.
					// Here, use a pure random choice in these cases
					return GetChoiceRandomly(board, action_analyzer, choice_getter);
				}

				int GetChoiceRandomly(
					engine::view::Board const& board,
					engine::ValidActionAnalyzer const& action_analyzer,
					ChoiceGetter const& choice_getter)
				{
					size_t count = choice_getter.Size();
					assert(count > 0);
					size_t idx = 0;
					size_t rand_idx = (size_t)(rand_() % count);
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
				std::mt19937 & rand_;

				std::vector<int> decision_;
				size_t decision_idx_;
				NeuralNetworkStateValueFunction state_value_func_;
			};
		}
	}
}
