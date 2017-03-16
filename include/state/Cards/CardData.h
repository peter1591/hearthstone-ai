#pragma once

#include <functional>
#include <vector>
#include <string>
#include "Utils/FuncPtrArray.h"
#include "state/Cards/EnchantableStates.h"
#include "state/Cards/EnchantmentAuxData.h"
#include "state/Cards/Contexts.h"
#include "FlowControl/aura/Handler.h"

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
				battlecry_target_getter(nullptr), battlecry(nullptr)
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

		public: // zone-changed callbacks invoked by state::State 
			typedef void AddedToPlayZoneCallback(ZoneChangedContext);
			Utils::FuncPtrArray<AddedToPlayZoneCallback*, 1> added_to_play_zone;

			typedef void AddedToDeckZoneCallback(ZoneChangedContext);
			Utils::FuncPtrArray<AddedToDeckZoneCallback*, 1> added_to_deck_zone;

		public:
			EnchantmentAuxData enchantment_aux_data;

		public:
			FlowControl::aura::Handler aura_handler;

		public:  // battlecry
			typedef bool BattlecryTargetGetter(FlowControl::Context::BattlecryTargetGetter); // true -> OK; false -> invalid to play
			BattlecryTargetGetter *battlecry_target_getter;

			typedef void BattlecryCallback(FlowControl::Context::BattleCry);
			BattlecryCallback *battlecry;

		public:
			typedef void DeathrattleCallback(FlowControl::Context::Deathrattle);
			typedef std::vector<DeathrattleCallback*> Deathrattles;
			Deathrattles deathrattles;

		public: // for hero type
			CardRef weapon_ref;
		};
	}
}