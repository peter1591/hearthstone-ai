#pragma once

#include <functional>
#include <vector>
#include <string>
#include "Cards/id-map.h"
#include "Utils/FuncPtrArray.h"
#include "state/Cards/EnchantableStates.h"
#include "state/Cards/Callbacks.h"
#include "engine/FlowControl/onplay/Handler.h"
#include "engine/FlowControl/deathrattle/Handler.h"
#include "engine/FlowControl/enchantment/Handler.h"

namespace state
{
	namespace Cards
	{
		class CardData
		{
		public:
			CardData() :
				card_id(::Cards::kInvalidCardId),
				card_type(kCardTypeInvalid), card_race(kCardRaceInvalid), card_rarity(kCardRarityInvalid),
				is_secret_card(false), overload(0),
				zone(kCardZoneInvalid), zone_position(-1),
				play_order(-1), damaged(0), just_played(false), num_attacks_this_turn(0), enchanted_states(),
				pending_destroy(false), used_this_turn(0), usable(true), armor(0),
				taunt(false), shielded(false), cant_attack(false), freezed(false),
				silenced(false),
				added_to_play_zone(), added_to_hand_zone(),
				enchantment_handler(), onplay_handler(), deathrattle_handler()
			{
				static_assert(kFieldChangeId == 2);
			}

			void RefCopy(CardData const& base) {
				static_assert(kFieldChangeId == 2);

				card_id = base.card_id;
				card_type = base.card_type;
				card_race = base.card_race;
				card_rarity = base.card_rarity;
				is_secret_card = base.is_secret_card;
				overload = base.overload;
				zone = base.zone;
				zone_position = base.zone_position;
				play_order = base.play_order;
				damaged = base.damaged;
				just_played = base.just_played;
				num_attacks_this_turn = base.num_attacks_this_turn;
				enchanted_states = base.enchanted_states;
				pending_destroy = base.pending_destroy;;
				used_this_turn = base.used_this_turn;
				usable = base.usable;
				armor = base.armor;
				taunt = base.taunt;
				shielded = base.shielded;
				cant_attack = base.cant_attack;
				freezed = base.freezed;
				silenced = base.silenced;
				added_to_play_zone = base.added_to_play_zone;
				added_to_hand_zone = base.added_to_hand_zone;
				enchantment_handler.RefCopy(base.enchantment_handler);
				onplay_handler = base.onplay_handler;
				deathrattle_handler.RefCopy(base.deathrattle_handler);
			}

			static constexpr int kFieldChangeId = 2;

			::Cards::CardId card_id;
			CardType card_type;
			CardRace card_race;
			CardRarity card_rarity;
			bool is_secret_card;
			int overload; // 0: no overload; 1: overload 1 crystal; etc.

			CardZone zone;
			int zone_position; // only meaningful for minions in play zone, and for hand cards

			int play_order;
			int damaged;
			bool just_played;
			int num_attacks_this_turn;
			EnchantableStates enchanted_states;

			bool pending_destroy;

			int used_this_turn; // for hero power
			bool usable; // for hero power

			int armor; // for hero

		public: // for minions
			bool taunt;
			bool shielded;
			bool cant_attack;
			bool freezed;

			bool silenced;

		public: // zone-changed callbacks invoked by state::State 
			Utils::FuncPtrArray<AddedToPlayZoneCallback*, 1> added_to_play_zone;
			Utils::FuncPtrArray<AddedToHandZoneCallback*, 1> added_to_hand_zone;

		public:
			engine::FlowControl::enchantment::Handler enchantment_handler;
			engine::FlowControl::onplay::Handler onplay_handler;
			engine::FlowControl::deathrattle::Handler deathrattle_handler;
		};
	}
}