#pragma once

#include "Cards/id-map.h"
#include "Cards/MinionCardUtils.h"
#include "Cards/EnchantmentCardBase.h"
#include "FlowControl/Dispatchers/Minions.h"

namespace Cards
{
	class Card_EX1_089 : MinionCardUtils
	{
	public:
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

	class Card_NEW1_038 : MinionCardUtils
	{
	public:
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
}

REGISTER_MINION_CARD_CLASS(Cards::ID_EX1_089, Cards::Card_EX1_089)
REGISTER_MINION_CARD_CLASS(Cards::ID_NEW1_038, Cards::Card_NEW1_038)
