#pragma once

#include <functional>
#include <list>
#include <string>
#include "State/Types.h"
#include "State/Cards/EnchantableStates.h"
#include "State/Cards/EnchantmentAuxData.h"
#include "State/Cards/AuraAuxData.h"
#include "State/Cards/AuraHandler.h"
#include "Cards/CardData.h"
#include "FlowControl/Context/BattlecryTargetGetter.h"

namespace FlowControl
{
	namespace Context
	{
		class AddedToPlayZone;
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
			typedef bool BattlecryTargetGetter(FlowControl::Context::BattlecryTargetGetter);
			typedef void BattlecryCallback(FlowControl::Context::BattleCry);
			typedef void AddedToPlayZoneCallback(FlowControl::Context::AddedToPlayZone &);

			typedef std::function<void(FlowControl::Context::Deathrattle &)> DeathrattleCallback;
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
			int zone_position;

			int play_order;
			int damaged;
			bool just_played;
			int num_attacks_this_turn;

			EnchantableStates enchantable_states;

			EnchantmentAuxData enchantment_aux_data;

			AuraAuxData aura_aux_data;

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