#ifndef GAME_ENGINE_CARDS_CARD_GVG_043
#define GAME_ENGINE_CARDS_CARD_GVG_043

#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/stages/helper.h"
#include "game-engine/board-objects/enchantment.h"

namespace GameEngine {
	namespace Cards {

		class Card_GVG_043
		{
		public:
			static constexpr int card_id = CARD_ID_GVG_043;

			// Weapon: Glaivezooka

			static void Weapon_BattleCry(GameEngine::Board & board, SlotIndex playing_side, GameEngine::Move::EquipWeaponData const& equip_weapon_data)
			{
				constexpr int attack_boost = 1;

				GameEngine::BoardObjects::Minions * playing_side_minions = nullptr;
				if (playing_side == SLOT_PLAYER_SIDE) playing_side_minions = &board.object_manager.player_minions;
				else if (playing_side == SLOT_OPPONENT_SIDE) playing_side_minions = &board.object_manager.opponent_minions;
				else throw std::runtime_error("invalid argument");

				const int minion_count = playing_side_minions->GetMinionCount();
				if (minion_count == 0) return;

				const int minion_chosen = board.random_generator.GetRandom(minion_count);

				auto & buff_target = playing_side_minions->GetMinion(minion_chosen);
				auto enchant = std::make_unique<BoardObjects::Enchantment_BuffMinion_C<attack_boost, 0, 0, false>>();
				buff_target.AddEnchantment(std::move(enchant), nullptr);
			}
		};

	} // namespace Cards
} // namespace GameEngine

#endif
