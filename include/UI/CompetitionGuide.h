#pragma once

#include <time.h>

#include <memory>
#include <random>
#include <sstream>

#include "state/State.h"
#include "FlowControl/FlowController.h"
#include "FlowControl/JsonSerializer.h"
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
		// hidden information (e.g., opponents hand cards).
		virtual void Think(state::State const& state, int threads, uint64_t iterations) = 0;

		virtual int GetMainAction() = 0;

		// TODO: Move BoardActionAnalyzer out of 'mcts' namespace
		virtual mcts::board::BoardActionAnalyzer GetActionApplier() = 0;

		virtual int GetSubAction(mcts::ActionType::Types action_type, mcts::board::ActionChoices action_choices) = 0;
	};

	class CompetitionRecorder
	{
	public:
		CompetitionRecorder(std::mt19937 & rand) : rand_(rand), json_() {}

		void Start() {
			json_.clear();
		}

		void RecordMainAction(state::State const& state, mcts::board::BoardActionAnalyzer const& analyzer, int action)
		{
			Json::Value obj;
			obj["type"] = "kMainAction";
			obj["board"] = FlowControl::JsonSerializer::Serialize(state);

			auto op = analyzer.GetMainOpType(action);
			obj["choice"] = GetMainOpString(op);

			json_.append(obj);
		}

		void RecordRandomAction(int exclusive_max, int action) {
			Json::Value obj;
			obj["type"] = "kRandom";
			obj["exclusive_max"] = exclusive_max;
			obj["choice"] = action;
			json_.append(obj);
		}

		void RecordManualAction(mcts::ActionType::Types action_type, mcts::board::ActionChoices action_choices, int action) {
			Json::Value obj;
			obj["type"] = GetActionTypeString(action_type);
			obj["choices_type"] = GetChoiceTypeString(action_choices);

			Json::Value choices;
			for (action_choices.Begin(); !action_choices.IsEnd(); action_choices.StepNext()) {
				Json::Value choice;
				choice = action_choices.Get();
				choices.append(choice);
			}
			obj["choices"] = choices;

			obj["choice"] = action;
			json_.append(obj);
		}

		void End(mcts::Result result) {
			{
				Json::Value obj;
				obj["type"] = "kEnd";
				obj["result"] = GetResultString(result);
				json_.append(obj);
			}

			assert(result != mcts::Result::kResultInvalid);
			assert(result != mcts::Result::kResultNotDetermined);

			time_t now;
			time(&now);

			struct tm timeinfo;
#ifdef _MSC_VER
			localtime_s(&timeinfo, &now);
#else
			localtime_r(&now, &timeinfo);
#endif

			char buffer[80];
			strftime(buffer, 80, "%Y%m%d-%H%M%S", &timeinfo);

			std::ostringstream ss;
			int postfix = rand() % 90000 + 10000;
			ss << buffer << "-" << postfix << ".json";
			std::string filename = ss.str();

			std::ofstream fs(filename, std::ofstream::trunc);
			Json::StyledStreamWriter json_writer;
			json_writer.write(fs, json_);
			fs.close();
		}

	private:
		std::string GetMainOpString(mcts::board::BoardActionAnalyzer::OpType op) {
			using mcts::board::BoardActionAnalyzer;
			switch (op) {
			case BoardActionAnalyzer::kInvalid: return "kInvalid";
			case BoardActionAnalyzer::kPlayCard: return "kPlayCard";
			case BoardActionAnalyzer::kAttack: return "kAttack";
			case BoardActionAnalyzer::kHeroPower: return "kHeroPower";
			case BoardActionAnalyzer::kEndTurn: return "kEndTurn";
			default:
				assert(false);
				return "Unknown!!!";
			}
		}

		std::string GetActionTypeString(mcts::ActionType type) {
			using mcts::ActionType;
			switch (type.GetType()) {
			case ActionType::kMainAction: return "kMainAction";
			case ActionType::kChooseHandCard: return "kChooseHandCard";
			case ActionType::kChooseAttacker: return "kChooseAttacker";
			case ActionType::kChooseDefender: return "kChooseDefender";
			case ActionType::kChooseMinionPutLocation: return "kChooseMinionPutLocation";
			case ActionType::kChooseTarget: return "kChooseTarget";
			case ActionType::kChooseOne: return "kChooseOne";

			case ActionType::kRandom:
				assert(false);
				return "kRandom";
			case ActionType::kInvalid:
				assert(false);
				return "kInvalid";
			default:
				assert(false);
				return "Unknown!!!";
			}
		}

		std::string GetChoiceTypeString(mcts::board::ActionChoices const& action_choices) {
			switch (action_choices.GetType()) {
			case mcts::board::ActionChoices::kChooseFromCardIds:
				return "kChooseFromCardIds";
			case mcts::board::ActionChoices::kChooseFromZeroToExclusiveMax:
				return "kChooseFromZeroToExclusiveMax";
			default:
				assert(false);
				return "Invalid";
			}
		}

		std::string GetResultString(mcts::Result result) {
			switch (result) {
			case mcts::Result::kResultWin:
				return "kResultWin";
			case mcts::Result::kResultLoss:
				return "kResultLoss";
			default:
				assert(false);
				return "kResultInvalid";
			}
		}

	private:
		std::mt19937 & rand_;
		Json::Value json_;
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

			void SetCallback(ICompetitor * cb) { cb_ = cb; }

			int GetNumber(mcts::ActionType::Types action_type, mcts::board::ActionChoices const& action_choices) final {
				int action = cb_->GetSubAction(action_type, action_choices);
				guide_.recorder_.RecordManualAction(action_type, action_choices, action);
				return action;
			}

		private:
			CompetitionGuide & guide_;
			ICompetitor * cb_;
		};

		CompetitionGuide(std::mt19937 & rand) :
			rand_(rand), random_callback_(*this), action_callback_(*this),
			first_(nullptr), second_(nullptr), recorder_(rand_)
		{}

		CompetitionGuide(CompetitionGuide const& rhs) = delete;
		CompetitionGuide & operator=(CompetitionGuide const& rhs) = delete;

		void SetFirstCompetitor(ICompetitor * first) { first_ = first; }
		void SetSecondCompetitor(ICompetitor * second) { second_ = second; }

		void Start(StartingStateGetter state_getter, int threads, uint64_t iterations) {
			state::State current_state = state_getter();
			assert(first_);
			assert(second_);
			
			recorder_.Start();

			mcts::Result result = mcts::Result::kResultInvalid;
			ICompetitor * next_competitor = nullptr;
			while (true) {
				std::cout << "Turn: " << current_state.GetTurn() << std::endl;

				if (current_state.GetCurrentPlayerId().IsFirst()) {
					next_competitor = first_;
				}
				else {
					assert(current_state.GetCurrentPlayerId().IsSecond());
					next_competitor = second_;
				}

				next_competitor->Think(current_state, threads, iterations);

				FlowControl::FlowContext flow_context;

				int main_action = next_competitor->GetMainAction();
				auto action_analyzer = next_competitor->GetActionApplier();

				recorder_.RecordMainAction(current_state, action_analyzer, main_action);
				action_callback_.SetCallback(next_competitor);

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