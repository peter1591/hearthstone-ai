#pragma once

#include "MCTS/Types.h"
#include "FlowControl/PlayerStateView.h"

namespace mcts
{
	namespace board
	{
		class ActionParameterGetter;
		class RandomGenerator;

		class BoardActionAnalyzer
		{
		private:
			enum MainActions
			{
				kActionPlayCard,
				kActionAttack,
				kActionHeroPower,
				kActionEndTurn,
				kActionMax // Should be at last
			};

		public:
			BoardActionAnalyzer() : op_map_(), op_map_size_(0), attackers_(), playable_cards_() {}

			int GetActionsCount(FlowControl::CurrentPlayerStateView const& board);

			Result ApplyAction(
				FlowControl::CurrentPlayerStateView board,
				int action,
				RandomGenerator & random,
				ActionParameterGetter & action_parameters);

			enum OpType {
				kInvalid,
				kPlayCard,
				kAttack,
				kHeroPower,
				kEndTurn
			};

			template <class Functor>
			void ForEachMainOp(Functor && functor) const {
				for (size_t i = 0; i < op_map_size_; ++i) {
					if (!functor(i, GetOpType(op_map_[i]))) return;
				}
			}
			OpType GetMainOpType(size_t choice) const {
				return GetMainOpType(op_map_[choice]);
			}

		private:
			typedef Result (BoardActionAnalyzer::*OpFunc)(FlowControl::CurrentPlayerStateView & board, RandomGenerator & random, ActionParameterGetter & action_parameters);
			Result PlayCard(FlowControl::CurrentPlayerStateView & board, RandomGenerator & random, ActionParameterGetter & action_parameters);
			Result Attack(FlowControl::CurrentPlayerStateView & board, RandomGenerator & random, ActionParameterGetter & action_parameters);
			Result HeroPower(FlowControl::CurrentPlayerStateView & board, RandomGenerator & random, ActionParameterGetter & action_parameters);
			Result EndTurn(FlowControl::CurrentPlayerStateView & board, RandomGenerator & random, ActionParameterGetter & action_parameters);

			OpType GetMainOpType(OpFunc func) const {
				if (func == &BoardActionAnalyzer::PlayCard) return kPlayCard;
				if (func == &BoardActionAnalyzer::Attack) return kAttack;
				if (func == &BoardActionAnalyzer::HeroPower) return kHeroPower;
				if (func == &BoardActionAnalyzer::EndTurn) return kEndTurn;
				return kInvalid;
			}

		private:
			std::array<OpFunc, kActionMax> op_map_;
			size_t op_map_size_;
			std::vector<int> attackers_;
			std::vector<size_t> playable_cards_;
		};
	}
}