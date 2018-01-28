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
		struct ChooseHandCardInfo {};
		struct MainOpInfo {};
		struct MinionPutLocationInfo {
			MinionPutLocationInfo(int minions) : minions(minions) {}
			int minions;
		};
		struct ChooseAttackerInfo {};
		struct ChooseDefenderInfo {
			ChooseDefenderInfo(std::vector<int> const& targets) : targets(targets) {}
			std::vector<int> targets;
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
			MainOpInfo,
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

		class ActionParameterChoser : public engine::IActionParameterGetter {
		public:
			ActionParameterChoser(std::vector<int> const& choices, size_t & choices_idx) :
				choices_(choices), choices_idx_(choices_idx), should_skip_(false)
			{}

			// should be called after GetNumber()
			bool HasFurtherChoices() const {
				return choices_idx_ <= choices_.size();
			}

			// should be called after GetNumber()
			bool ShouldSkip() const { return should_skip_; }

			int GetNumber(engine::ActionType::Types action_type, engine::ActionChoices & action_choices) final {
				should_skip_ = false;
				if (action_type != engine::ActionType::kMainAction)
				{
					assert(action_choices.Size() > 0);
					if (action_choices.Size() == 1) {
						should_skip_ = true;
						return 0;
					}
				}
				return GetNextChoice(0, action_choices.Size());
			}

		private:
			int GetNextChoice(int min, int exclusive_max) {
				if (choices_idx_ >= choices_.size()) {
					++choices_idx_;
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
			std::vector<int> const& choices_;
			size_t & choices_idx_;
			bool should_skip_;
		};

		class ActionParameterCallback : public engine::FlowControl::IActionParameterGetter {
		public:
			ActionParameterCallback(std::vector<int> const& choices, size_t & choices_idx, CallbackInfo & result) :
				result_(result), choser_(choices, choices_idx)
			{}
			
			void Initialize(state::State const& game_state) {
				result_ = NullInfo();
				choser_.Initialize(game_state);
			}

			MainOpType ChooseMainOp() {
				auto ret = choser_.ChooseMainOp();
				if (ShouldRecord()) result_ = MainOpInfo();
				return ret;
			}

			int ChooseHandCard() {
				auto ret = choser_.ChooseHandCard();
				if (ShouldRecord()) result_ = ChooseHandCardInfo();
				return ret;
			}

			state::CardRef GetAttacker() {
				auto ret = choser_.GetAttacker();
				if (ShouldRecord()) result_ = ChooseAttackerInfo();
				return ret;
			}

			state::CardRef GetDefender(std::vector<int> const& targets) {
				auto ret = choser_.GetDefender(targets);
				if (ShouldRecord()) result_ = ChooseDefenderInfo(targets);
				return ret;
			}

			int GetMinionPutLocation(int minions) {
				auto ret = choser_.GetMinionPutLocation(minions);
				if (ShouldRecord()) result_ = MinionPutLocationInfo(minions);
				return ret;
			}

			state::CardRef GetSpecifiedTarget(
				state::State & state, state::CardRef card_ref,
				std::vector<state::CardRef> const& targets) {
				auto ret = choser_.GetSpecifiedTarget(state, card_ref, targets);
				if (ShouldRecord()) result_ = GetSpecifiedTargetInfo(card_ref, targets);
				return ret;
			}

			Cards::CardId ChooseOne(std::vector<Cards::CardId> const& cards) {
				auto ret = choser_.ChooseOne(cards);
				if (ShouldRecord()) result_ = ChooseOneInfo(cards);
				return ret;
			}

		private:
			bool ShouldRecord() const {
				if (!std::holds_alternative<NullInfo>(result_)) return false;
				if (choser_.ShouldSkip()) return false;
				if (choser_.HasFurtherChoices()) return false;
				return true;
			}

		private:
			CallbackInfo & result_;
			ActionParameterChoser choser_;
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
			engine::Result result;
			return ApplyChoices(game_state, result);
		}

		CallbackInfo ApplyChoices(state::State & game_state, engine::Result & result) const
		{
			CallbackInfo info = NullInfo();

			size_t choices_idx = 0;

			RandomCallback rand_cb;
			ActionParameterCallback action_cb(choices_, choices_idx, info);

			engine::FlowControl::FlowContext flow_context;
			flow_context.SetCallback(rand_cb, action_cb);

			while (choices_.size() > choices_idx) {
				action_cb.Initialize(game_state);
				result = engine::FlowControl::FlowController(game_state, flow_context).PerformAction();
			}

			return info;
		}

	private:
		std::vector<int> choices_;
	};
}