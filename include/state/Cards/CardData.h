#pragma once

#include <functional>
#include <vector>
#include <string>
#include "Utils/FuncPtrArray.h"
#include "state/Cards/EnchantableStates.h"
#include "state/Cards/Contexts.h"
#include "FlowControl/aura/Handler.h"
#include "FlowControl/battlecry/Handler.h"
#include "FlowControl/deathrattle/Handler.h"
#include "FlowControl/enchantment/Handler.h"

namespace state
{
	namespace Cards
	{
		class CardData
		{
		public:
			CardData() :
				card_id(-1), card_type(kCardTypeInvalid), card_race(kCardRaceInvalid), card_rarity(kCardRarityInvalid),
				zone(kCardZoneInvalid), zone_position(-1),
				play_order(-1), damaged(0), just_played(false), num_attacks_this_turn(0),
				silenced(false)
			{
			}

			int card_id;
			CardType card_type;
			CardRace card_race;
			CardRarity card_rarity;

			CardZone zone;
			int zone_position; // only meaningful for minions in play zone, and for hand cards

			int play_order;
			int damaged;
			bool just_played;
			int num_attacks_this_turn;
			EnchantableStates enchanted_states;

		public: // for hero type
			CardRef weapon_ref;

		public: // for minions
			bool silenced;

		public: // zone-changed callbacks invoked by state::State 
			typedef void AddedToPlayZoneCallback(ZoneChangedContext);
			Utils::FuncPtrArray<AddedToPlayZoneCallback*, 1> added_to_play_zone;

			typedef void AddedToDeckZoneCallback(ZoneChangedContext);
			Utils::FuncPtrArray<AddedToDeckZoneCallback*, 1> added_to_deck_zone;

		public:
			FlowControl::enchantment::Handler enchantment_handler;
			FlowControl::aura::Handler aura_handler;
			FlowControl::battlecry::Handler battlecry_handler;
			FlowControl::deathrattle::Handler deathrattle_handler;
		};
	}
}