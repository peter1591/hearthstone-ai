#pragma once

#include <random>
#include <string>

#include <json/json.h>

#include <state/State.h>

namespace judge
{
	class JsonRecorder
	{
	public:
		JsonRecorder(std::mt19937 & rand) : rand_(rand), json_() {}

		void Start() {
			json_.clear();
		}

		void RecordMainAction(state::State const& state, FlowControl::MainOpType op)
		{
			Json::Value obj;
			obj["type"] = "kMainAction";
			obj["board"] = FlowControl::JsonSerializer::Serialize(state);
			obj["choice"] = FlowControl::utils::GetMainOpString(op);

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

		void End(FlowControl::Result result) {
			assert(result != FlowControl::kResultInvalid);
			assert(result != FlowControl::kResultNotDetermined);

			{
				Json::Value obj;
				obj["type"] = "kEnd";
				obj["result"] = GetResultString(result);
				json_.append(obj);
			}

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

		std::string GetResultString(FlowControl::Result result) {
			switch (result) {
			case FlowControl::kResultDraw:
				return "kResultDraw";
			case FlowControl::kResultFirstPlayerWin:
				return "kResultFirstPlayerWin";
			case FlowControl::kResultSecondPlayerWin:
				return "kResultSecondPlayerWin";
			default:
				assert(false);
				return "kResultInvalid";
			}
		}

	private:
		std::mt19937 & rand_;
		Json::Value json_;
	};
}