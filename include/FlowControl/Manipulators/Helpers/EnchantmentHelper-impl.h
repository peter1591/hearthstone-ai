#pragma once

#include <assert.h>
#include "FlowControl/Manipulate.h"
#include "FlowControl/Manipulators/Helpers/EnchantmentHelper.h"
#include "Cards/Database.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			inline void EnchantmentHelper::SetHealthToMaxHP() {
				FlowControl::Manipulate(state_, flow_context_).Card(card_ref_).Internal_SetDamage().SetAsUndamaged();
			}
		}
	}
}