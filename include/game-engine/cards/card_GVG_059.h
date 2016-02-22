#pragma once

#include <memory>
#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/stages/helper.h"
#include "game-engine/board-objects/minion.h"
#include "game-engine/board-objects/enchantment.h"

namespace GameEngine {
	namespace Cards {

		class Card_GVG_059
		{
		public:
			static constexpr int card_id = CARD_ID_GVG_059;

			// Coghammer
			static void Weapon_BattleCry(GameEngine::Board &board, SlotIndex playing_side, GameEngine::Move::EquipWeaponData const& equip_weapon_data)
			{
				GameEngine::BoardObjects::Minions * minions = nullptr;

				if (playing_side == SLOT_PLAYER_SIDE) minions = &board.object_manager.player_minions;
				else if (playing_side == SLOT_OPPONENT_SIDE) minions = &board.object_manager.opponent_minions;
				else throw std::runtime_error("invalid argument");

				const int minion_count = minions->GetMinionCount();
				if (minion_count == 0) return; // no minion to buff

				const int r = board.random_generator.GetRandom(minion_count);

				auto & buff_target = minions->GetMinion(r);
				constexpr int buff_flags =
					(1 << BoardObjects::MinionStat::FLAG_SHIELD) |
					(1 << BoardObjects::MinionStat::FLAG_TAUNT);
				auto enchant = std::make_unique<BoardObjects::Enchantment_BuffMinion_C<0, 0, buff_flags, false>>();
				buff_target.AddEnchantment(std::move(enchant), nullptr);
			}
		};

	} // namespace Cards
} // namespace GameEngine
