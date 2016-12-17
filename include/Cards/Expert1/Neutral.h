#pragma once

#include "Cards/id-map.h"
#include "Cards/MinionCardBase.h"
#include "Cards/MinionCardUtils.h"
#include "Cards/EnchantmentCardBase.h"
#include "FlowControl/Dispatchers/Minions.h"

namespace Cards
{
	class Card_EX1_089 : public MinionCardBase<Card_EX1_089>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_EX1_089;

		Card_EX1_089()
		{
			battlecry = [](auto& context) {
				AnotherPlayer(context).GainEmptyCrystal();
			};
		}
	};

	class Card_NEW1_038_Enchant : public EnchantmentCardBase
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

	class Card_NEW1_038 : public MinionCardBase<Card_NEW1_038>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_NEW1_038;

		Card_NEW1_038()
		{
			added_to_play_zone = [](auto& context) {
				state::CardRef self = context.card_ref_;
				context.state_.event_mgr.PushBack<state::Events::EventTypes::OnTurnEnd>(
					[self](auto& controller, auto& context) {
					if (!IsAlive(context, self)) return controller.Remove();
					Manipulate(context).Card(self).Enchant().Add(Card_NEW1_038_Enchant());
				});
			};
		}
	};

	class Card_EX1_020 : public MinionCardBase<Card_EX1_020>
	{
	public:
		static constexpr int id = Cards::ID_EX1_020;

		Card_EX1_020()
		{
			this->enchantable_states.mechanics.shield = true;
		}
	};

	class Card_CS1_069 : public MinionCardBase<Card_CS1_069>
	{
	public:
		static constexpr int id = Cards::ID_CS1_069;

		Card_CS1_069()
		{
			this->enchantable_states.mechanics.taunt = true;
		}
	};
}

REGISTER_MINION_CARD_CLASS(Cards::Card_EX1_089)
REGISTER_MINION_CARD_CLASS(Cards::Card_NEW1_038)
REGISTER_MINION_CARD_CLASS(Cards::Card_EX1_020)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS1_069)
