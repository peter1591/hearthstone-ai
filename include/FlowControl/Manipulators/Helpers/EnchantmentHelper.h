#pragma once

#include <assert.h>
#include <memory>
#include <utility>
#include "State/Cards/Card.h"
#include "State/Cards/EnchantmentAuxData.h"
#include "FlowControl/Context/EnchantmentAfterAdded.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			class EnchantmentHelper
			{
			public:
				EnchantmentHelper(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) :
					state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card)
				{
				}

				template <typename T>
				decltype(auto) Add(T&& enchantment)
				{
					state::Cards::EnchantmentAuxData & data = card_.GetMutableEnchantmentAuxDataGetter().Get();

					data.need_update = true;
					auto ret = data.enchantments.PushBack(std::forward<T>(enchantment));
					if (enchantment.after_added_callback) {
						enchantment.after_added_callback(Context::EnchantmentAfterAdded(state_, flow_context_, card_ref_, ret));
					}
					return ret;
				}

				template <typename EnchantmentType, typename T>
				decltype(auto) Remove(T&& id)
				{
					state::Cards::EnchantmentAuxData & data = card_.GetMutableEnchantmentAuxDataGetter().Get();

					data.need_update = true;
					return data.enchantments.Remove<EnchantmentType>(std::forward<T>(id));
				}

				void Update();

			private:
				void ChangeMinionPlayer(state::PlayerIdentifier player);

			private:
				state::State & state_;
				FlowContext & flow_context_;
				state::CardRef card_ref_;
				state::Cards::Card & card_;
			};
		}
	}
}