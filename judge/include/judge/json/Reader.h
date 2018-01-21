#pragma once

#include "json/json.h"
#include "neural_net/NeuralNetwork.h"

namespace judge
{
	namespace json
	{
		class NeuralNetRefInputGetter : public neural_net::IInputGetter
		{
		public:
			NeuralNetRefInputGetter(Json::Value const& obj) : obj_(obj) {
			}

			double GetField(
				neural_net::FieldSide field_side,
				neural_net::FieldType field_type,
				int arg1 = 0) const override final
			{
				if (field_side == neural_net::FieldSide::kCurrent) {
					return GetSideField(field_type, arg1, obj_["current_player"]);
				}
				else if (field_side == neural_net::FieldSide::kOpponent) {
					return GetSideField(field_type, arg1, obj_["opponent_player"]);
				}
				throw std::runtime_error("invalid side");
			}

		private:
			double GetSideField(neural_net::FieldType field_type, int arg1, Json::Value const& side) const {
				switch (field_type) {
				case neural_net::FieldType::kResourceCurrent:
				case neural_net::FieldType::kResourceTotal:
				case neural_net::FieldType::kResourceOverload:
				case neural_net::FieldType::kResourceOverloadNext:
					return GetResourceField(field_type, arg1, side["resource"]);

				case neural_net::FieldType::kHeroHP:
				case neural_net::FieldType::kHeroArmor:
					return GetHeroField(field_type, arg1, side["hero"]);

				case neural_net::FieldType::kMinionCount:
				case neural_net::FieldType::kMinionHP:
				case neural_net::FieldType::kMinionMaxHP:
				case neural_net::FieldType::kMinionAttack:
				case neural_net::FieldType::kMinionAttackable:
				case neural_net::FieldType::kMinionTaunt:
				case neural_net::FieldType::kMinionShield:
				case neural_net::FieldType::kMinionStealth:
					return GetMinionsField(field_type, arg1, side["minions"]);

				case neural_net::FieldType::kHandCount:
				case neural_net::FieldType::kHandPlayable:
				case neural_net::FieldType::kHandCost:
					return GetHandField(field_type, arg1, side["hand"]);

				case neural_net::FieldType::kHeroPowerPlayable:
					return GetHeroPowerField(field_type, arg1, side["hero_power"]);

				default:
					throw std::runtime_error("unknown field type");
				}
			}

			double GetResourceField(neural_net::FieldType field_type, int arg1, Json::Value const& resource) const {
				switch (field_type) {
				case neural_net::FieldType::kResourceCurrent:
					return resource["current"].asInt();
				case neural_net::FieldType::kResourceTotal:
					return resource["total"].asInt();
				case neural_net::FieldType::kResourceOverload:
					return resource["overload_current"].asInt();
				case neural_net::FieldType::kResourceOverloadNext:
					return resource["overload_next"].asInt();
				default:
					throw std::runtime_error("unknown field type");
				}
			}

			double GetHeroField(neural_net::FieldType field_type, int arg1, Json::Value const& hero) const {
				switch (field_type) {
				case neural_net::FieldType::kHeroHP:
					return hero["hp"].asInt();
				case neural_net::FieldType::kHeroArmor:
					return hero["armor"].asInt();
				default:
					throw std::runtime_error("unknown field type");
				}
			}

			double GetMinionsField(neural_net::FieldType field_type, int minion_idx, Json::Value const& minions) const {
				switch (field_type) {
				case neural_net::FieldType::kMinionCount:
					return minions.size();
				case neural_net::FieldType::kMinionHP:
					return minions[minion_idx]["hp"].asInt();
				case neural_net::FieldType::kMinionMaxHP:
					return minions[minion_idx]["max_hp"].asInt();
				case neural_net::FieldType::kMinionAttack:
					return minions[minion_idx]["attack"].asInt();
				case neural_net::FieldType::kMinionAttackable:
					return minions[minion_idx]["attackable"].asBool();
				case neural_net::FieldType::kMinionTaunt:
					return minions[minion_idx]["taunt"].asBool();
				case neural_net::FieldType::kMinionShield:
					return minions[minion_idx]["shield"].asBool();
				case neural_net::FieldType::kMinionStealth:
					return minions[minion_idx]["stealth"].asBool();
				default:
					throw std::runtime_error("unknown field type");
				}
			}

			double GetHandField(neural_net::FieldType field_type, int hand_idx, Json::Value const& hand) const {
				switch (field_type) {
				case neural_net::FieldType::kHandCount:
					return hand.size();
				case neural_net::FieldType::kHandPlayable:
					return hand[hand_idx]["playable"].asBool();
				case neural_net::FieldType::kHandCost:
					return hand[hand_idx]["cost"].asInt();
				default:
					throw std::runtime_error("unknown field type");
				}
			}

			double GetHeroPowerField(neural_net::FieldType field_type, int arg1, Json::Value const& hero_power) const {
				switch (field_type) {
				case neural_net::FieldType::kHeroPowerPlayable:
					return hero_power["playable"].asBool();
				default:
					throw std::runtime_error("unknown field type");
				}
			}

		private:
			Json::Value const& obj_;
		};

		class NeuralNetInputGetter : public neural_net::IInputGetter
		{
		public:
			NeuralNetInputGetter(Json::Value const& obj) 
				: obj_(obj) // copy
			{}

			double GetField(
				neural_net::FieldSide field_side,
				neural_net::FieldType field_type,
				int arg1 = 0) const override final
			{
				return NeuralNetRefInputGetter(obj_).GetField(field_side, field_type, arg1);
			}

		private:
			Json::Value obj_;
		};

		class Reader
		{
		public:
			template <class Callback, class GetterType = NeuralNetInputGetter>
			void Parse(Json::Value const& obj, Callback&& cb) {
				std::string result = GetResult(obj);
				for (Json::ArrayIndex idx = 0; idx < obj.size(); ++idx) {
					if (obj[idx]["type"].asString() == "kMainAction") {
						Json::Value const& board = obj[idx]["board"];

						int label = IsCurrentPlayerWin(board, result) ? 1 : -1;
						cb(GetterType(board), label);
					}
				}
			}

		private:
			std::string GetResult(Json::Value const& obj) {
				for (Json::ArrayIndex idx = 0; idx < obj.size(); ++idx) {
					if (obj[idx]["type"].asString() == "kEnd") {
						return obj[idx]["result"].asString();
					}
				}
				throw std::runtime_error("Cannot find win player");
			}

			bool IsResultWin(std::string const& win_player) {
				if (win_player == "kResultFirstPlayerWin") return true;
				if (win_player == "kResultSecondPlayerWin") return false;
				if (win_player == "kResultDraw") return false;
				throw std::runtime_error("Failed to parse winning player");
			}

			bool IsCurrentPlayerWin(Json::Value const& board, std::string const& result) {
				std::string current_player = board["current_player_id"].asString();

				bool current_player_is_first = false;
				if (current_player == "kFirstPlayer") current_player_is_first = true;
				else if (current_player == "kSecondPlayer") current_player_is_first = false;
				else throw std::runtime_error("Failed to parse current player");

				// Note: AI is always helping first player
				bool win_player_is_first = IsResultWin(result);

				return current_player_is_first == win_player_is_first;
			}
		};
	}
}