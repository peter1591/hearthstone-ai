#pragma once

#include <array>
#include <vector>

#include "FlowControl/MainOp.h"
#include "FlowControl/PlayerStateView.h"

namespace FlowControl
{
	class ValidActionAnalyzer
	{
	public:
		ValidActionAnalyzer() : op_map_(), op_map_size_(0), attackers_(), playable_cards_() {}

		ValidActionAnalyzer(ValidActionAnalyzer const& rhs) :
			op_map_(rhs.op_map_),
			op_map_size_(rhs.op_map_size_),
			attackers_(rhs.attackers_),
			playable_cards_(rhs.playable_cards_)
		{}

		ValidActionAnalyzer & operator=(ValidActionAnalyzer const& rhs) {
			op_map_ = rhs.op_map_;
			op_map_size_ = rhs.op_map_size_;
			attackers_ = rhs.attackers_;
			playable_cards_ = rhs.playable_cards_;
			return *this;
		}

		void Reset() { op_map_size_ = 0; }

		void Analyze(FlowControl::CurrentPlayerStateView const& board) {
			Reset();

			playable_cards_.clear();
			board.ForEachPlayableCard([&](size_t idx) {
				playable_cards_.push_back(idx);
				return true;
			});
			if (!playable_cards_.empty()) {
				op_map_[op_map_size_] = FlowControl::MainOpType::kMainOpPlayCard;
				++op_map_size_;
			}

			attackers_.clear();
			board.ForEachAttacker([&](int idx) {
				attackers_.push_back(idx);
				return true;
			});
			if (!attackers_.empty()) {
				op_map_[op_map_size_] = FlowControl::MainOpType::kMainOpAttack;
				++op_map_size_;
			}

			if (board.CanUseHeroPower()) {
				op_map_[op_map_size_] = FlowControl::MainOpType::kMainOpHeroPower;
				++op_map_size_;
			}

			op_map_[op_map_size_] = FlowControl::MainOpType::kMainOpEndTurn;
			++op_map_size_;
		}

		auto const& GetMainActions() const { return op_map_; }
		int GetMainActionsCount() const { return (int)op_map_size_; }
		auto const& GetAttackers() const { return attackers_; }
		auto const& GetPlayableCards() const { return playable_cards_; }

		template <class Functor>
		void ForEachMainOp(Functor && functor) const {
			for (size_t i = 0; i < op_map_size_; ++i) {
				if (!functor(i, GetMainOpType(op_map_[i]))) return;
			}
		}
		FlowControl::MainOpType GetMainOpType(size_t choice) const {
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

		int GetEncodedAttackerIndex(size_t idx) const {
			return attackers_[idx];
		}

	private:
		std::array<FlowControl::MainOpType, FlowControl::MainOpType::kMainOpMax> op_map_;
		size_t op_map_size_;
		std::vector<int> attackers_;
		std::vector<size_t> playable_cards_;
	};
}