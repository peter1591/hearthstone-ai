#pragma once

#include <functional>
#include <vector>
#include <string>
#include "Utils/FuncPtrArray.h"
#include "state/Cards/EnchantableStates.h"
#include "state/Cards/EnchantmentAuxData.h"
#include "state/Cards/aura/AuraAuxData.h"
#include "state/Cards/aura/AuraHandler.h"
#include "state/Cards/Contexts.h"

namespace state
{
	namespace Cards
	{
		class CardData
		{
		public:
			typedef void AddedToPlayZoneCallback(ZoneChangedContext);
			typedef void AddedToDeckZoneCallback(ZoneChangedContext);

			typedef bool BattlecryTargetGetter(FlowControl::Context::BattlecryTargetGetter&&); // true -> OK; false -> invalid to play
			typedef void BattlecryCallback(FlowControl::Context::BattleCry&&);

			typedef void (*DeathrattleCallback)(FlowControl::Context::Deathrattle);
			typedef std::vector<DeathrattleCallback> Deathrattles;

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

		public: // callbacks managed by state::State 
			Utils::FuncPtrArray<AddedToPlayZoneCallback*, 1> added_to_play_zone;
			Utils::FuncPtrArray<AddedToDeckZoneCallback*, 1> added_to_deck_zone;

		public:
			EnchantableStates enchantable_states;
			EnchantmentAuxData enchantment_aux_data;

		public:
			aura::AuraHandler aura_handler;
			aura::AuraAuxData aura_aux_data;

		public:
			BattlecryTargetGetter *battlecry_target_getter;
			BattlecryCallback *battlecry;
			Deathrattles deathrattles;

		public: // for hero type
			CardRef weapon_ref;
		};
	}
}