#pragma once

#include <array>
#include <vector>

#include "state/State.h"
#include "engine/MainOp.h"
#include "engine/ActionTargets.h"

namespace engine {
	namespace FlowControl
	{
		class ValidActionGetter;
	}

	class ValidActionAnalyzer
	{
	public:
		ValidActionAnalyzer() : 
			op_map_(), op_map_size_(0), attackers_(), playable_cards_(), action_targets_()
		{}

		void Reset() { op_map_size_ = 0; }

		void Analyze(state::State const& state);
		void Analyze(FlowControl::ValidActionGetter const& getter);

		auto const& GetMainActions() const { return op_map_; }
		int GetMainActionsCount() const { return (int)op_map_size_; }
		auto const& GetAttackers() const { return attackers_; }
		auto const& GetPlayableCards() const { return playable_cards_; }

		auto GetCardRefFromTargetIndex(int idx) const {
			return action_targets_.GetCardRef(idx);
		}

		template <class Functor>
		void ForEachMainOp(Functor && functor) const {
			for (size_t i = 0; i < op_map_size_; ++i) {
				if (!functor(i, GetMainOpType(op_map_[i]))) return;
			}
		}
		engine::MainOpType GetMainOpType(size_t choice) const {
			return op_map_[choice];
		}
		template <class Functor>
		void ForEachPlayableCard(Functor && functor) const {
			for (auto hand_idx : playable_cards_) {
				if (!functor(hand_idx)) break;
			}
		}
		size_t GetPlaybleCard(size_t idx) const {
			return playable_cards_[idx];
		}

		int GetAttackerIndex(size_t idx) const {
			return attackers_[idx];
		}

	private:
		std::array<engine::MainOpType, engine::MainOpType::kMainOpMax> op_map_;
		size_t op_map_size_;
		std::vector<int> attackers_;
		std::vector<size_t> playable_cards_;
		ActionTargets action_targets_;
	};
}