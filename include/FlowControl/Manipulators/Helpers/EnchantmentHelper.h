#pragma once

#include <assert.h>
#include <memory>
#include <utility>
#include "state/Cards/Card.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			class EnchantmentHelper
			{
			public:
				EnchantmentHelper(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref) :
					state_(state), flow_context_(flow_context), card_ref_(card_ref)
				{
				}

				template <typename T> auto Add() {
					return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T>(state_);
				}
				template <template <int> typename T> auto Add(int v) {
					assert(v >= 0);
					switch (v) {
					case 0: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<0>>(state_);
					case 1: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<1>>(state_);
					case 2: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<2>>(state_);
					case 3: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<3>>(state_);
					case 4: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<4>>(state_);
					case 5: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<5>>(state_);
					case 6: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<6>>(state_);
					case 7: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<7>>(state_);
					case 8: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<8>>(state_);
					case 9: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<9>>(state_);
					case 10: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<10>>(state_);
					case 11: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<11>>(state_);
					case 12: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<12>>(state_);
					case 13: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<13>>(state_);
					case 14: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<14>>(state_);
					case 15: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<15>>(state_);
					case 16: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<16>>(state_);
					case 17: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<17>>(state_);
					case 18: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<18>>(state_);
					case 19: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<19>>(state_);
					case 20: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<20>>(state_);
					}
					assert(false); // need more switch-case
					return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment<T<20>>(state_);
				}

				void SetHealthToMaxHP();

				template <typename T> auto AddAuraEnchantment() {
					return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackAuraEnchantment<T>(state_);
				}

				template <typename T> auto Remove(T&& id) {
					return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().Remove(std::forward<T>(id));
				}

				void Update(bool allow_hp_reduce = false) {
					return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().Update(state_, flow_context_, card_ref_, allow_hp_reduce);
				}

			private:
				state::State & state_;
				FlowControl::FlowContext & flow_context_;
				state::CardRef card_ref_;
			};
		}
	}
}