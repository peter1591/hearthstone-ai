#pragma once

#include <json/json.h>
#include <string>

#include "state/State.h"
#include "state/JsonSerializer.h"
#include "FlowControl/ValidActionGetter.h"

namespace FlowControl
{
	class JsonSerializer
	{
	public:
		static Json::Value Serialize(state::State const& state) {
			Json::Value obj;

			Json::Value state_obj = state::JsonSerializer::Serialize(state);

			std::string current_player_str = state_obj["current_player"].asString();
			obj["current_player_id"] = current_player_str;
			obj["turn"] = state_obj["turn"];

			if (current_player_str == state::JsonSerializer::GetPlayerString(state::kPlayerFirst)) {
				obj["current_player"] = state_obj["first_player"];
				obj["opponent_player"] = state_obj["second_player"];
			}
			else {
				assert(current_player_str == state::JsonSerializer::GetPlayerString(state::kPlayerSecond));
				obj["current_player"] = state_obj["second_player"];
				obj["opponent_player"] = state_obj["first_player"];
			}

			AddPlayableCardInfo(obj["current_player"]["hand"], state);
			AddPlayableHeroPowerInfo(obj["current_player"]["hero_power"], state);
			AddAttackableInfo(obj["current_player"], state);

			return obj;
		}

	private:
		static void AddPlayableCardInfo(Json::Value & obj, state::State const& state) {
			for (Json::Value::ArrayIndex idx = 0; idx < obj.size(); ++idx) {
				obj[idx]["playable"] = false;
			}
			ValidActionGetter valid_action_getter(state);
			valid_action_getter.ForEachPlayableCard([&](size_t idx) {
				auto json_idx = (Json::Value::ArrayIndex)idx;
				obj[json_idx]["playable"] = true;
				return true;
			});
		}

		static void AddPlayableHeroPowerInfo(Json::Value & obj, state::State const& state) {
			ValidActionGetter valid_action_getter(state);
			obj["playable"] = valid_action_getter.CanUseHeroPower();
		}

		static void AddAttackableInfo(Json::Value & obj, state::State const& state) {
			for (Json::Value::ArrayIndex idx = 0; idx < obj["minions"].size(); ++idx) {
				obj["minions"][idx]["attackable"] = false;
			}
			obj["hero"]["attackable"] = false;

			ValidActionGetter valid_action_getter(state);
			valid_action_getter.ForEachAttacker([&](int encoded_idx) {
				if (encoded_idx >= 0 && encoded_idx <= 6) {
					// minion
					auto json_idx = (Json::Value::ArrayIndex)encoded_idx;
					obj["minions"][json_idx]["attackable"] = true;
				}
				else {
					// hero
					assert(encoded_idx == 7);
					obj["hero"]["attackable"] = true;
				}
				return true;
			});
		}
	};
}
