#pragma once

#include "Cards/id-map.h"
#include "Cards/MinionCardBase.h"
#include "Cards/MinionCardUtils.h"
#include "Cards/EnchantmentCardBase.h"

namespace Cards
{
	class Card_CS2_189 : public MinionCardBase<Card_CS2_189>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS2_189;

		Card_CS2_189()
		{
			battlecry = [](auto& context) {
				state::CardRef ref = context.GetBattleCryTarget(Targets().Targetable().Enemy());
				Damage(context).Target(ref).Amount(1);
			};
		}
	};
}

REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_189)
