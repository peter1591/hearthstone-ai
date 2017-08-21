#pragma once

#include <memory>
#include <random>

#include "state/State.h"
#include "FlowControl/FlowController.h"
#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/board/BoardActionAnalyzer.h"
#include "MCTS/board/RandomGenerator.h"

namespace ui
{
	class ICompetitor {
	public:
		using StartingStateGetter = std::function<state::State()>;

		enum MainActions
		{
			kActionPlayCard,
			kActionAttack,
			kActionHeroPower,
			kActionEndTurn
		};

		virtual ~ICompetitor() {}

		// TODO: The underlying truth state is passed. It means the competitor can acquire
		// hidden information like opponents hand cards.
		virtual void Think(state::State const& state, int think_time) = 0;

		virtual std::unique_ptr<mcts::board::IActionParameterGetter> GetActionGetter() = 0;

		virtual int GetMainAction() = 0;

		// TODO: Move BoardActionAnalyzer out of 'mcts' namespace
		virtual mcts::board::BoardActionAnalyzer GetActionApplier() = 0;
	};

	class CompetitionRecorder
	{
	public:
		void Start();

		void RecordMainAction(state::State const& state, mcts::board::BoardActionAnalyzer const& analyzer, int action);

		void RecordRandomAction(int exclusive_max, int action);

		void RecordManualAction(mcts::ActionType::Types action_type, mcts::board::ActionChoices const& action_choices, int action);

		void End(mcts::Result result) {
			assert(result != mcts::Result::kResultInvalid);
			assert(result != mcts::Result::kResultNotDetermined);

			// TODO
		}

	private:
	};

	class CompetitionGuide
	{
	public:
		using StartingStateGetter = std::function<state::State()>;

		class RandomCallback : public mcts::board::IRandomGenerator {
		public:
			RandomCallback(CompetitionGuide & guide) : guide_(guide) {}
			
			int Get(int exclusive_max) final {
				int action = guide_.rand_() % exclusive_max;
				guide_.recorder_.RecordRandomAction(exclusive_max, action);
				return action;
			}

		private:
			CompetitionGuide & guide_;
		};

		class ActionCallback : public mcts::board::IActionParameterGetter {
		public:
			ActionCallback(CompetitionGuide & guide) : guide_(guide), cb_(nullptr) {}

			ActionCallback(ActionCallback const&) = delete;
			ActionCallback & operator=(ActionCallback const&) = delete;

			void SetCallback(mcts::board::IActionParameterGetter * cb) { cb_ = cb; }

			int GetNumber(mcts::ActionType::Types action_type, mcts::board::ActionChoices const& action_choices) final {
				int action = cb_->GetNumber(action_type, action_choices);
				guide_.recorder_.RecordManualAction(action_type, action_choices, action);
				return action;
			}

		private:
			CompetitionGuide & guide_;
			mcts::board::IActionParameterGetter * cb_;
		};

		CompetitionGuide(std::mt19937 & rand) :
			rand_(rand), random_callback_(*this), action_callback_(*this),
			first_(nullptr), second_(nullptr), recorder_()
		{}

		CompetitionGuide(CompetitionGuide const& rhs) = delete;
		CompetitionGuide & operator=(CompetitionGuide const& rhs) = delete;

		void SetFirstCompetitor(ICompetitor * first) { first_ = first; }
		void SetSecondCompetitor(ICompetitor * second) { second_ = second; }

		void Start(StartingStateGetter state_getter, int think_time) {
			state::State current_state = state_getter();
			assert(first_);
			assert(second_);
			
			recorder_.Start();

			mcts::Result result = mcts::Result::kResultInvalid;
			ICompetitor * next_competitor = nullptr;
			while (true) {
				if (current_state.GetCurrentPlayerId().IsFirst()) {
					next_competitor = first_;
				}
				else {
					assert(current_state.GetCurrentPlayerId().IsSecond());
					next_competitor = second_;
				}

				next_competitor->Think(current_state, think_time);

				FlowControl::FlowContext flow_context;

				int main_action = next_competitor->GetMainAction();
				auto action_cb = next_competitor->GetActionGetter();
				auto action_analyzer = next_competitor->GetActionApplier();

				recorder_.RecordMainAction(current_state, action_analyzer, main_action);
				action_callback_.SetCallback(action_cb.get());

				result = action_analyzer.ApplyAction(
					flow_context, FlowControl::CurrentPlayerStateView(current_state),
					main_action,
					random_callback_, action_callback_);

				assert(result != mcts::Result::kResultInvalid);
				if (result != mcts::Result::kResultNotDetermined) {
					break;
				}
			}

			recorder_.End(result);
		}

	private:
		std::mt19937 & rand_;
		RandomCallback random_callback_;
		ActionCallback action_callback_;
		ICompetitor * first_;
		ICompetitor * second_;
		CompetitionRecorder recorder_;
	};
}