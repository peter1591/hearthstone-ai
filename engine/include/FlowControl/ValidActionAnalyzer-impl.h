#pragma once

#include "FlowControl/ValidActionAnalyzer.h"
#include "FlowControl/ValidActionGetter.h"

namespace FlowControl
{
	inline void ValidActionAnalyzer::Analyze(ValidActionGetter const& getter) {
		Reset();

		playable_cards_.clear();
		getter.ForEachPlayableCard([&](size_t idx) {
			playable_cards_.push_back(idx);
			return true;
		});
		if (!playable_cards_.empty()) {
			op_map_[op_map_size_] = FlowControl::MainOpType::kMainOpPlayCard;
			++op_map_size_;
		}

		attackers_.clear();
		getter.ForEachAttacker([&](int idx) {
			attackers_.push_back(idx);
			return true;
		});
		if (!attackers_.empty()) {
			op_map_[op_map_size_] = FlowControl::MainOpType::kMainOpAttack;
			++op_map_size_;
		}

		attacker_indics_ = getter.GetAttackerIndics();

		if (getter.CanUseHeroPower()) {
			op_map_[op_map_size_] = FlowControl::MainOpType::kMainOpHeroPower;
			++op_map_size_;
		}

		op_map_[op_map_size_] = FlowControl::MainOpType::kMainOpEndTurn;
		++op_map_size_;
	}
}