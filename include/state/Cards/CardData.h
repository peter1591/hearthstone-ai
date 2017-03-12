#pragma once

#include <functional>
#include <list>
#include <string>
#include "state/Types.h"
#include "state/Cards/EnchantableStates.h"
#include "state/Cards/EnchantmentAuxData.h"
#include "state/Cards/aura/AuraAuxData.h"
#include "state/Cards/AuraHandler.h"
#include "Cards/CardData.h"
#include "FlowControl/Contexts.h"

namespace state
{
	class State;

	namespace Cards
	{
		class CardData
		{
		public:
			typedef bool BattlecryTargetGetter(FlowControl::Context::BattlecryTargetGetter&&);
			typedef void BattlecryCallback(FlowControl::Context::BattleCry&&);
			typedef void AddedToPlayZoneCallback(FlowControl::Context::AddedToPlayZone);

			typedef void (*DeathrattleCallback)(FlowControl::Context::Deathrattle);
			typedef std::list<DeathrattleCallback> Deathrattles;

			CardData() :
				card_id(-1), card_type(kCardTypeInvalid), card_race(kCardRaceInvalid), card_rarity(kCardRarityInvalid),
				zone(kCardZoneInvalid), zone_position(-1),
				play_order(-1), damaged(0), just_played(false), num_attacks_this_turn(0),
				added_to_play_zone(nullptr), battlecry_target_getter(nullptr), battlecry(nullptr)
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

			EnchantableStates enchantable_states;

			EnchantmentAuxData enchantment_aux_data;

			aura::AuraAuxData aura_aux_data;

		public: // callbacks, not subject to change when game flows
			AddedToPlayZoneCallback *added_to_play_zone;
			BattlecryTargetGetter *battlecry_target_getter;
			BattlecryCallback *battlecry;
			Deathrattles deathrattles;
			AuraHandler aura_handler;

		public: // for hero type
			CardRef weapon_ref;
		};
	}
}