#pragma once

#include <functional>

#include "state/State.h"
#include "engine/FlowControl/FlowController.h"

namespace engine
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
		class RandomCallback : public engine::FlowControl::IRandomGenerator {
		public:
			int Get(int exclusive_max) { return 0; }
		};

		class ActionParameterCallback : public engine::IActionParameterGetter {
		public:
			ActionParameterCallback(CallbackInfo & result, std::vector<int> const& choices, size_t & choices_idx) :
				result_(result), choices_(choices), choices_idx_(choices_idx)
			{}

			int GetNumber(engine::ActionType::Types action_type, engine::ActionChoices const& action_choices) final {
				if (action_type != engine::ActionType::kMainAction)
				{
					assert(action_choices.Size() > 0);
					if (action_choices.Size() == 1) return 0;
				}
				return GetNextChoice(0, action_choices.Size());
			}

		private:
			int GetNextChoice(int min, int exclusive_max) {
				if (choices_idx_ >= choices_.size()) {
					return min; // just an arbitrary valid choice
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

			engine::FlowControl::FlowContext flow_context;
			flow_context.SetCallback(rand_cb, action_cb);

			while (choices_.size() > choices_idx) {
				action_cb.Initialize(game_state);
				engine::FlowControl::FlowController(game_state, flow_context).PerformAction();
			}

			return info;
		}

	private:
		std::vector<int> choices_;
	};
}