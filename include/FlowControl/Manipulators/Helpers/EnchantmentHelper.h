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
				EnchantmentHelper(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) :
					state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card)
				{
				}

				template <typename T> auto Add() {
					return card_.GetMutableEnchantmentHandler().PushBackNormalEnchantment<T>(state_);
				}

				template <typename T> auto AddAuraEnchantment() {
					return card_.GetMutableEnchantmentHandler().PushBackAuraEnchantment<T>(state_);
				}

				template <typename T> auto Remove(T&& id) {
					return card_.GetMutableEnchantmentHandler().Remove(std::forward<T>(id));
				}

				void Update(bool allow_hp_reduce = false) {
					return card_.GetMutableEnchantmentHandler().Update(state_, flow_context_, card_ref_, card_, allow_hp_reduce);
				}

			private:
				state::State & state_;
				FlowControl::FlowContext & flow_context_;
				state::CardRef card_ref_;
				state::Cards::Card & card_;
			};
		}
	}
}