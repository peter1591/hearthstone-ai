#pragma once

#include <functional>

#include "state/State.h"
#include "FlowControl/FlowController.h"
#include "mcts/board/RandomGenerator.h"
#include "mcts/board/BoardActionAnalyzer.h"
#include "judge/IActionParameterGetter.h"

namespace mcts
{
	namespace inspector
	{
		class ActionApplyHelper
		{
		public:
			struct NullInfo {};
			struct ChooseHandCardInfo {
				ChooseHandCardInfo() {}
			};
			struct MinionPutLocationInfo {
				MinionPutLocationInfo(int minions) : minions(minions) {}
				int minions;
			};
			struct ChooseAttackerInfo {
				ChooseAttackerInfo(std::vector<int> targets) : targets(targets) {}
				std::vector<int> targets;
			};
			struct ChooseDefenderInfo {
				ChooseDefenderInfo(std::vector<state::CardRef> targets) : targets(targets) {}
				std::vector<state::CardRef> targets;
			};
			struct GetSpecifiedTargetInfo {
				GetSpecifiedTargetInfo(state::CardRef card_ref, std::vector<state::CardRef> targets) :
					card_ref(card_ref), targets(targets)
				{}
				state::CardRef card_ref;
				std::vector<state::CardRef> targets;
			};
			struct ChooseOneInfo {
				ChooseOneInfo(std::vector<Cards::CardId> const& cards) : cards(cards) {}
				std::vector<Cards::CardId> cards;
			};
			struct ChooseRandomInfo {
				ChooseRandomInfo(int exclusive_max) : exclusive_max(exclusive_max) {}
				int exclusive_max;
			};
			using CallbackInfo = std::variant<
				NullInfo, // no callback called
				ChooseHandCardInfo,
				MinionPutLocationInfo,
				ChooseAttackerInfo,
				ChooseDefenderInfo,
				GetSpecifiedTargetInfo,
				ChooseOneInfo>;

			using StartBoardGetter = std::function<state::State(int)>;

		private:
			class RandomCallback : public state::IRandomGenerator {
			public:
				int Get(int exclusive_max) { return 0; }
			};

			class ActionParameterCallback : public judge::IActionParameterGetter {
			public:
				ActionParameterCallback(CallbackInfo & result, std::vector<int> const& choices, size_t & choices_idx) :
					result_(result), choices_(choices), choices_idx_(choices_idx), main_op_idx_(-1)
				{}

				void SetMainOpIdx(int main_op_idx) { main_op_idx_ = main_op_idx; }

				int GetNumber(ActionType::Types action_type, ActionChoices const& action_choices) {
					return GetNextChoice(0, action_choices.Size());
				}

			private:
				int GetNextChoice(int min, int exclusive_max) {
					if (choices_idx_ >= choices_.size()) {
						return min;
					}

					int choice = choices_[choices_idx_];
					++choices_idx_;
					(void)exclusive_max;
					assert(choice >= min);
					assert(choice < exclusive_max);
					return choice;
				}

			private:
				CallbackInfo & result_;
				std::vector<int> const& choices_;
				size_t & choices_idx_;
				int main_op_idx_;
				std::vector<size_t> const* playable_cards_;
			};

		public:
			ActionApplyHelper() : choices_() {}

			void AppendChoice(int choice) {
				choices_.push_back(choice);
			}

			void ClearChoices() {
				choices_.clear();
			}

			template <class StartBoardGetter>
			CallbackInfo ApplyChoices(StartBoardGetter && start_board_getter) const
			{
				state::State game_state = start_board_getter();
				return ApplyChoices(game_state);
			}

			CallbackInfo ApplyChoices(state::State & game_state) const
			{
				CallbackInfo info = NullInfo();

				size_t choices_idx = 0;

				RandomCallback rand_cb;
				ActionParameterCallback action_cb(info, choices_, choices_idx);
				action_cb.Initialize(game_state);

				FlowControl::FlowContext flow_context;
				flow_context.SetCallback(rand_cb, action_cb);

				while (choices_.size() > choices_idx) {
					int main_op_idx = choices_[choices_idx];
					++choices_idx;

					mcts::board::BoardActionAnalyzer analyzer;
					FlowControl::CurrentPlayerStateView state_view(game_state);
					analyzer.Prepare(state_view);
					action_cb.SetMainOpIdx(main_op_idx);

					FlowControl::FlowController(game_state, flow_context).PerformOperation();
				}

				return info;
			}

		private:
			std::vector<int> choices_;
		};
	}
}