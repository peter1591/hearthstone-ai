#pragma once

namespace FlowControl
{
	namespace enchantment
	{
		namespace detail
		{
			class UpdateDecider {
			public:
				UpdateDecider() : item_changed_(false), need_update_after_turn_(-1), force_update_items_(0) {}

				void AddItem() { item_changed_ = true; }
				void RemoveItem() { item_changed_ = true; }
				void AddValidUntilTurn(int valid_until_turn);
				void AddForceUpdateItem() { ++force_update_items_; }
				void RemoveForceUpdateItem() { --force_update_items_; assert(force_update_items_ >= 0); }

				void FinishedUpdate(state::State const& state);
				bool NeedUpdate(state::State const& state) const;

			private:
				bool item_changed_;
				int need_update_after_turn_;
				int force_update_items_;
			};
		}
	}
}