#pragma once

#include "json/value.h"
#include "game-engine/board.h"
#include "game-engine/card-database.h"

namespace JsonBoardFinder
{
	class BoardComparator
	{
	public:
		static bool IsEqual(GameEngine::Board const& board, Json::Value const& json_board)
		{
			if (!ComparePlayer(board.player, json_board["player"])) return false;
			if (!ComparePlayer(board.opponent, json_board["opponent"])) return false;

			return true;
		}

		static bool IsPlayingSide(GameEngine::Player const& player)
		{
			GameEngine::Player * playing_player = &player.board.player;
			if (player.board.GetStageType() == GameEngine::STAGE_TYPE_OPPONENT) {
				playing_player = &player.board.opponent;
			}
			return &player == playing_player;
		}

		static bool ComparePlayer(GameEngine::Player const& player, Json::Value const& json)
		{
			if (!CompareCrystal(player.stat.crystal, json["crystal"], IsPlayingSide(player))) return false;
			
			if (player.stat.fatigue_damage != json["fatigue"].asInt()) return false;

			if (!CompareWeapon(player.hero.GetHeroData().weapon, json["weapon"])) return false;
			if (!CompareHero(player.hero, json["hero"])) return false;

			if (!CompareMinions(player.minions, json["minions"])) return false;

			if (player.IsPlayerSide() && !ComparePlayerHand(player.hand, json)) return false;
			if (!player.IsPlayerSide() && !CompareOpponentHand(player.hand, json)) return false;

			// TODO: check player spell damage
			//      CURRENT_SPELLPOWER
			// TODO: secrets

			if (IsPlayingSide(player)) {
				if (player.hero.GetWeaponAttack() != json["weapon"]["attack"].asInt()) return false;
				if (player.hero.GetAttack() != json["hero"]["attack"].asInt()) return false;
			}

			return true;
		}

		static bool ComparePlayerHand(GameEngine::Hand const& hand, Json::Value const& json)
		{
			Json::Value const& json_hand = json["hand"];

			if (hand.GetCount() != json_hand["cards"].size()) return false;

			auto json_hand_it = json_hand["cards"].begin();
			for (size_t i = 0; i < hand.GetCount(); ++i)
			{
				auto const& hand_card = hand.GetCard(i);
				
				std::string json_card_id_s = json_hand_it->asString();
				int json_card_id = GameEngine::CardDatabase::GetInstance().GetCardIdFromOriginalId(json_card_id_s);

				if (hand_card.id != json_card_id) return false;

				json_hand_it++;
			}

			return true;
		}

		static bool CompareOpponentHand(GameEngine::Hand const& hand, Json::Value const& json)
		{
			if (hand.GetCount() != json["hand"]["cards"].size()) return false;

			// only compare hand card count for opponent
			return true;
		}

		static bool CompareMinions(GameEngine::Minions const& minions, Json::Value const& json)
		{
			if (minions.GetMinionCount() != (int)json.size()) return false;

			auto minion_it = minions.GetMinionsIteratorWithIndexAtBegin((GameEngine::SlotIndex)(minions.GetPlayer().side + 1));
			auto json_it = json.begin();
			while (true) {
				if (minion_it.IsEnd()) break;
				if (json_it == json.end()) throw std::runtime_error("both iterators should have same number of items");

				if (!CompareMinion(*minion_it, *json_it)) return false;

				minion_it.GoToNext();
				json_it++;
			}

			return true;
		}

		static bool IsPlayingSide(GameEngine::Minion const& minion)
		{
			GameEngine::Player * playing_player = &minion.GetBoard().player;
			if (minion.GetBoard().GetStageType() == GameEngine::STAGE_TYPE_OPPONENT) {
				playing_player = &minion.GetBoard().opponent;
			}
			return &minion.GetMinions().GetPlayer() == playing_player;
		}

		static bool CompareMinion(GameEngine::Minion const& minion, Json::Value const& json)
		{
			int card_id = GameEngine::CardDatabase::GetInstance().GetCardIdFromOriginalId(json["card_id"].asString());
			
			if (minion.GetMinion().card_id != card_id) return false;

			if (IsPlayingSide(minion)) {
				// Summoned minions (e.g., from totemic call) seems to not have this flag
				// --> just check the 'exhausted' property
				//if (minion.GetMinion().summoned_this_turn != json["summoned_this_turn"].asBool()) return false;

				if (minion.GetMinion().attacked_times != json["attacks_this_turn"].asInt()) return false;
			}

			int max_hp = json["max_hp"].asInt();
			int hp = max_hp - json["damage"].asInt();
			if (minion.GetHP() != hp) return false;
			if (minion.GetMaxHP() != max_hp) return false;
			if (minion.GetAttack() != json["attack"].asInt()) return false;
			if (minion.GetMinion().stat.GetSpellDamage() != json["spellpower"].asInt()) return false;

			if (minion.GetMinion().stat.IsCharge() != (json["status"]["charge"].asInt() > 0)) return false;
			if (minion.GetMinion().stat.IsTaunt() != (json["status"]["taunt"].asInt() > 0)) return false;
			if (minion.GetMinion().stat.IsShield() != (json["status"]["divine_shield"].asInt() > 0)) return false;
			if (minion.GetMinion().stat.IsStealth() != (json["status"]["stealth"].asInt() > 0)) return false;
			if (minion.GetMinion().stat.IsForgetful() != (json["status"]["forgetful"].asInt() > 0)) return false;
			if (minion.GetMinion().stat.IsFreezeAttacker() != (json["status"]["freeze"].asInt() > 0)) return false;
			if (minion.GetMinion().stat.IsFreezed() != (json["status"]["frozen"].asInt() > 0)) return false;
			if (minion.GetMinion().stat.IsPoisonous() != (json["status"]["poisonous"].asInt() > 0)) return false;
			if (minion.GetMinion().stat.IsWindFury() != (json["status"]["windfury"].asInt() > 0)) return false;

			if (IsPlayingSide(minion)) {
				// only compare exhausted if the minion have attack
				if (minion.GetAttack() > 0 && !minion.GetMinion().stat.IsFreezed()) {
					if (minion.Attackable() == json["exhausted"].asBool()) return false;
				}
			}

			// TODO: check silenced: json["silenced"]
			return true;
		}

		static bool IsPlayingHero(GameEngine::Hero const& hero)
		{
			GameEngine::Hero * playing_hero = &hero.GetBoard().player.hero;
			if (hero.GetBoard().GetStageType() == GameEngine::STAGE_TYPE_OPPONENT) {
				playing_hero = &hero.GetBoard().opponent.hero;
			}
			return &hero == playing_hero;
		}

		static bool CompareHero(GameEngine::Hero const& hero, Json::Value const& json)
		{
			int max_hp = json["max_hp"].asInt();
			int hp = max_hp - json["damage"].asInt();

			if (hero.GetHeroData().max_hp != max_hp) return false;
			if (hero.GetHeroData().hp != hp) return false;
			if (hero.GetHeroData().armor != json["armor"].asInt()) return false;

			// fields only matters when it's his turn
			if (IsPlayingHero(hero)) {
				if (hero.GetHeroData().attacked_times != json["attacks_this_turn"].asInt()) return false;
				if (hero.GetHeroData().freezed != (json["status"]["frozen"].asInt() != 0)) return false;
			}

			int hero_power_card_id = GameEngine::CardDatabase::GetInstance().GetCardIdFromOriginalId(json["hero_power"]["card_id"].asString());
			if (hero.GetHeroData().hero_power.card_id != hero_power_card_id) return false;

			// fields only matters when it's his turn
			if (IsPlayingHero(hero)) {
				if (hero.GetHeroData().hero_power.used_this_turn != json["hero_power"]["used"].asBool()) return false;
			}

			return true;
		}

		static bool CompareWeapon(GameEngine::Weapon const& weapon, Json::Value const& json)
		{
			if (weapon.IsValid() != json["equipped"].asBool()) return false;

			int weapon_card_id = GameEngine::CardDatabase::GetInstance().GetCardIdFromOriginalId(json["card_id"].asString());
			if (weapon.card_id != weapon_card_id) return false;

			int durability = json["durability"].asInt() - json["damage"].asInt();
			if (weapon.durability != durability) return false;
			
			return true;
		}

		static bool CompareCrystal(GameEngine::PlayerStat::Crystal const& crystal, Json::Value const& json, bool is_playing_side)
		{
			int total = json["total"].asInt();
			int used = json["used"].asInt();
			int this_turn = json["this_turn"].asInt();
			int overload = json["overload"].asInt();
			int overload_next_turn = json["overload_next_turn"].asInt();

			if (crystal.GetTotal() != total) return false;

			if (is_playing_side) {
				if (crystal.GetCurrent() != total + this_turn - used) return false;
				if (crystal.GetLocked() != overload) return false;
				if (crystal.GetLockedNextTurn() != overload_next_turn) return false;
			}

			return true;
		}
	};
} // namespace JsonBoardFinder