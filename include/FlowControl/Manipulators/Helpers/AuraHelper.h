#pragma once

#include "State/Cards/AuraAuxData.h"
#include "State/State.h"
#include "FlowControl/Dispatchers/Minions.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			class AuraHelper
			{
			public:
				struct AuraEnchantment
				{
					static constexpr EnchantmentTiers tier = kEnchantmentAura;

					AuraEnchantment() : after_added_callback(nullptr) {}

					state::Cards::Enchantments::ApplyFunctor apply_functor;
					state::Cards::Enchantments::AfterAddedCallback *after_added_callback;

					template <typename T> void Set(T&& t) {
						apply_functor = std::move(t.apply_functor);
						after_added_callback = std::move(t.after_added_callback);
					}
				};

			public:
				AuraHelper(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) :
					state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card)
				{
				}

				void Set(int id) { card_.SetAuraId(id); }

				void Update();

			private:
				state::State & state_;
				FlowContext & flow_context_;
				state::CardRef card_ref_;
				state::Cards::Card & card_;
			};
		}
	}
}