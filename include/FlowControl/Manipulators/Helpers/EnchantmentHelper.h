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

				template <typename T> auto Add(T&& enchantment) {
					return card_.GetMutableEnchantmentHandler().Add(std::forward<T>(enchantment));
				}

				template <typename EnchantmentType, typename T> auto Remove(T&& id) {
					return card_.GetMutableEnchantmentHandler().Remove<EnchantmentType>(std::forward<T>(id));
				}

				void Update() {
					return card_.GetMutableEnchantmentHandler().Update(state_, flow_context_, card_ref_, card_);
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