#pragma once

#include <assert.h>
#include "engine/FlowControl/Manipulate.h"
#include "engine/FlowControl/Manipulators/Helpers/EnchantmentHelper.h"
#include "engine/FlowControl/Manipulators/detail/DamageSetter.h"
#include "Cards/Database.h"

namespace engine {
	namespace FlowControl
	{
		namespace Manipulators
		{
			namespace Helpers
			{
				inline void EnchantmentHelper::SetHealthToMaxHP() {
					detail::DamageSetter(state_, card_ref_).SetAsUndamaged();
				}

				inline void EnchantmentHelper::Update(bool allow_hp_reduce) {
					return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().Update(state_, flow_context_, card_ref_, allow_hp_reduce);
				}
			}
		}
	}
}