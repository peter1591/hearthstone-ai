#pragma once

#include <assert.h>
#include <string>

namespace FlowControl
{
	namespace utils
	{
		enum MainOpType {
			kMainOpPlayCard,
			kMainOpAttack,
			kMainOpHeroPower,
			kMainOpEndTurn,
			kMainOpInvalid, kMainOpMax = kMainOpInvalid
		};

		std::string GetMainOpString(MainOpType op) {
			switch (op) {
			case MainOpType::kMainOpInvalid: return "kInvalid";
			case MainOpType::kMainOpPlayCard: return "kPlayCard";
			case MainOpType::kMainOpAttack: return "kAttack";
			case MainOpType::kMainOpHeroPower: return "kHeroPower";
			case MainOpType::kMainOpEndTurn: return "kEndTurn";
			default:
				assert(false);
				return "Unknown!!!";
			}
		}
	}
}