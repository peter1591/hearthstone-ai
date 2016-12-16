#pragma once

#include "Cards/id-map.h"
#include "Cards/MinionCardBase.h"
#include "Cards/MinionCardUtils.h"
#include "Cards/EnchantmentCardBase.h"
#include "FlowControl/Dispatchers/Minions.h"

namespace Cards
{
	class Card_EX1_089 : public MinionCardBase, MinionCardUtils
	{
	public:
		Card_EX1_089() : MinionCardBase(Cards::ID_EX1_089, 3, 4, 4) {}

		static void BattleCry(FlowControl::Context::BattleCry & context)
		{
			AnotherPlayer(context).GainEmptyCrystal();
		}
	};

	class Card_NEW1_038_Enchant : public Cards::EnchantmentCardBase
	{
	public:
		static constexpr EnchantmentTiers tier = kEnchantmentTier1;

		Card_NEW1_038_Enchant()
		{
			apply_functor = [](auto& stats) {
				++stats.attack;
				++stats.max_hp;
			};
		}
	};

	class Card_NEW1_038 : public MinionCardBase, MinionCardUtils
	{
	public:
		Card_NEW1_038() : MinionCardBase(Cards::ID_NEW1_038, 8, 7, 7)
		{

		}

		static void AfterSummoned(FlowControl::Context::AfterSummoned & context)
		{
			state::CardRef self = context.card_ref_;

			context.state_.event_mgr.PushBack<state::Events::EventTypes::OnTurnEnd>(
				[self](auto& controller, auto& context) {
				if (!IsAlive(context, self)) return controller.Remove();
				Manipulate(context).Card(self).Enchant().Add(Card_NEW1_038_Enchant());
			});
		}
	};

	class Card_EX1_020 : public MinionCardBase
	{
	public:
		Card_EX1_020() : MinionCardBase(Cards::ID_EX1_020, 3, 3, 1)
		{
			this->enchantable_states.mechanics.shield = true;
		}
	};

	class Card_CS1_069 : public MinionCardBase
	{
	public:
		Card_CS1_069() : MinionCardBase(Cards::ID_CS1_069, 5, 3, 6)
		{
			this->enchantable_states.mechanics.taunt = true;
		}
	};
}

REGISTER_MINION_CARD_CLASS(Cards::ID_EX1_089, Cards::Card_EX1_089)
REGISTER_MINION_CARD_CLASS(Cards::ID_NEW1_038, Cards::Card_NEW1_038)
REGISTER_MINION_CARD_CLASS(Cards::ID_EX1_020, Cards::Card_EX1_020)
REGISTER_MINION_CARD_CLASS(Cards::ID_CS1_069, Cards::Card_CS1_069)
