#pragma once

#include "engine/ValidActionAnalyzer.h"
#include "engine/FlowControl/ValidActionGetter.h"

#include "engine/ActionTargets-impl.h"

namespace engine {
	inline void ValidActionAnalyzer::Analyze(state::State const& game_state) {
		return Analyze(FlowControl::ValidActionGetter(game_state));
	}
	
	inline void ValidActionAnalyzer::Analyze(FlowControl::ValidActionGetter const& getter) {
		Reset();

		action_targets_.Analyze(getter);

		playable_cards_.clear();
		getter.ForEachPlayableCard([&](size_t idx) {
			playable_cards_.push_back(idx);
			return true;
		});
		if (!playable_cards_.empty()) {
			op_map_[op_map_size_] = engine::MainOpType::kMainOpPlayCard;
			++op_map_size_;
		}

		attackers_.clear();
		getter.ForEachAttacker([&](int idx) {
			attackers_.push_back(idx);
			return true;
		});
		if (!attackers_.empty()) {
			op_map_[op_map_size_] = engine::MainOpType::kMainOpAttack;
			++op_map_size_;
		}

		if (getter.CanUseHeroPower()) {
			op_map_[op_map_size_] = engine::MainOpType::kMainOpHeroPower;
			++op_map_size_;
		}

		op_map_[op_map_size_] = engine::MainOpType::kMainOpEndTurn;
		++op_map_size_;
	}
}