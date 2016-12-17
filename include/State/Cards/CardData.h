#pragma once

#include <functional>
#include <list>
#include <string>
#include "State/Types.h"
#include "State/Cards/EnchantableStates.h"
#include "State/Cards/EnchantmentAuxData.h"
#include "State/Cards/AuraAuxData.h"
#include "Cards/CardData.h"

namespace FlowControl
{
	namespace Context
	{
		class BattleCry;
		class Deathrattle;
	}
}

namespace state
{
	class State;

	namespace Cards
	{
		class CardData
		{
		public:
			typedef void BattlecryCallback(FlowControl::Context::BattleCry &);

			typedef std::function<void(FlowControl::Context::Deathrattle &)> DeathrattleCallback;
			typedef std::list<DeathrattleCallback> Deathrattles;

			CardData() :
				card_id(-1), card_type(kCardTypeInvalid), play_order(-1), zone(kCardZoneInvalid),
				zone_position(-1), damaged(0), just_played(false), num_attacks_this_turn(0),
				aura_id(-1), battlecry(nullptr)
			{
			}

			int card_id;
			CardType card_type;
			int play_order;

			CardZone zone;
			int zone_position;

			int damaged;
			bool just_played;
			int num_attacks_this_turn;

			EnchantableStates enchantable_states;

			EnchantmentAuxData enchantment_aux_data;

			int aura_id;
			AuraAuxData aura_aux_data;

			BattlecryCallback *battlecry;
			Deathrattles deathrattles;

		public: // for hero type
			CardRef weapon_ref;
		};
	}
}