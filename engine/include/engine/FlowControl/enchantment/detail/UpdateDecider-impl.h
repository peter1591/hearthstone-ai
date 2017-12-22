#pragma once

#include "engine/FlowControl/enchantment/detail/UpdateDecider.h"

namespace engine {
	namespace FlowControl
	{
		namespace enchantment
		{
			namespace detail
			{
				inline void UpdateDecider::AddValidUntilTurn(int valid_until_turn) {
					if (valid_until_turn < 0) return;
					if (need_update_after_turn_ < 0) {
						need_update_after_turn_ = valid_until_turn;
						return;
					}
					assert(need_update_after_turn_ == valid_until_turn); // provided that all enchantment are 'valid until the end of the turn'
				}

				inline void UpdateDecider::FinishedUpdate(state::State const & state)
				{
					item_changed_ = false;

					if (need_update_after_turn_ < 0) return;
					if (state.GetTurn() > need_update_after_turn_) {
						need_update_after_turn_ = -1;
					}
				}

				inline bool UpdateDecider::NeedUpdate(state::State const & state) const
				{
					if (item_changed_) return true;

					if (force_update_items_ > 0) return true;

					if (need_update_after_turn_ >= 0) {
						if (state.GetTurn() > need_update_after_turn_) {
							return true;
						}
					}

					return false;
				}
			}
		}
	}
}