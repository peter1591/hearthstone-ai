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