#pragma once

#include <memory>
#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/stages/helper.h"
#include "game-engine/board-objects/minion.h"
#include "game-engine/enchantments/enchantment.h"

namespace GameEngine {
	namespace Cards {

		class Card_GVG_059
		{
		public:
			static constexpr int card_id = CARD_ID_GVG_059;

			// Coghammer
			static void Weapon_BattleCry(GameEngine::Player &equipping_player, GameEngine::Move::EquipWeaponData const&)
			{
				const int minion_count = equipping_player.minions.GetMinionCount();
				if (minion_count == 0) return; // no minion to buff

				const int r = equipping_player.board.random_generator.GetRandom(minion_count);

				auto & buff_target = equipping_player.minions.GetMinion(r);
				constexpr int buff_flags =
					(1 << MinionStat::FLAG_SHIELD) |
					(1 << MinionStat::FLAG_TAUNT);
				auto enchant = std::make_unique<Enchantment_BuffMinion_C<0, 0, 0, buff_flags, false>>();
				buff_target.enchantments.Add(std::move(enchant), nullptr);
			}
		};

	} // namespace Cards
} // namespace GameEngine
