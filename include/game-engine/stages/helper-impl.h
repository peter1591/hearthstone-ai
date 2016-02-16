#pragma once

#include "game-engine/stages/helper.h"
#include "game-engine/cards/common.h"

namespace GameEngine
{
	inline bool StageHelper::PlayerDrawCard(Board & board)
	{
		if (board.player_deck.GetCards().empty()) {
			StageHelper::Fatigue(board, SLOT_PLAYER_SIDE);
			return StageHelper::CheckHeroMinionDead(board);
		}

		Card draw_card = board.player_deck.Draw();

		if (board.player_hand.GetCount() < 10) {
			board.player_hand.AddCard(draw_card);
		}
		else {
			// hand can have maximum of 10 cards
			// TODO: distroy card (trigger deathrattle?)
		}

		return false;
	}

	inline bool StageHelper::OpponentDrawCard(Board & board)
	{
		if (board.opponent_cards.GetDeckCount() == 0) {
			StageHelper::Fatigue(board, SLOT_OPPONENT_SIDE);
			return StageHelper::CheckHeroMinionDead(board);
		}

		if (board.opponent_cards.GetHandCount() < 10) {
			board.opponent_cards.DrawFromDeckToHand();
		}
		else {
			// hand can have maximum of 10 cards
			// TODO: distroy card (trigger deathrattle?)
		}

		return false;
	}

	inline void StageHelper::DealDamage(GameEngine::Board & board, SlotIndex taker_idx, int damage, bool poisonous)
	{
		return StageHelper::DealDamage(board.object_manager.GetObject(board, taker_idx), damage, poisonous);
	}

	inline void StageHelper::DealDamage(GameEngine::BoardObjects::BoardObject taker, int damage, bool poisonous)
	{
		taker->TakeDamage(damage, poisonous);
	}

	inline SlotIndex StageHelper::GetTargetForForgetfulAttack(GameEngine::Board & board, SlotIndex origin_attacked)
	{
		const bool player_side = SlotIndexHelper::IsPlayerSide(origin_attacked);

		int possible_targets = 1 - 1; // +1 for the hero, -1 for the original attack target
		if (player_side) possible_targets += board.object_manager.GetPlayerMinionsCount();
		else possible_targets += board.object_manager.GetOpponentMinionsCount();

		if (possible_targets == 0)
		{
			// no other target available --> attack original target
			return origin_attacked;
		}

		if (board.random_generator.GetRandom() % 2 == 0) {
			// forgetful effect does not triggered
			return origin_attacked;
		}

		int r = board.random_generator.GetRandom() % possible_targets;

		if (player_side) {
			if (origin_attacked != SLOT_PLAYER_HERO) {
				// now we have a chance to attack the hero, take care of it first
				if (r == possible_targets - 1) {
					return SLOT_PLAYER_HERO;
				}
			}
			// now we can only hit the minions
			return SlotIndexHelper::GetPlayerMinionIndex(r);
		}
		else {
			if (origin_attacked != SLOT_OPPONENT_HERO) {
				// now we have a chance to attack the hero, take care of it first
				if (r == possible_targets - 1) {
					return SLOT_OPPONENT_HERO;
				}
			}
			// now we can only hit the minions
			return SlotIndexHelper::GetOpponentMinionIndex(r);
		}
	}

	inline void StageHelper::HandleAttack(GameEngine::Board & board, GameEngine::SlotIndex attacker_idx, GameEngine::SlotIndex attacked_idx)
	{
		// TODO: trigger secrets

		auto attacker = board.object_manager.GetObject(board, attacker_idx);

		if (attacker->IsForgetful()) {
			attacked_idx = StageHelper::GetTargetForForgetfulAttack(board, attacked_idx);
		}

		auto attacked = board.object_manager.GetObject(board, attacked_idx);
		StageHelper::DealDamage(attacked, attacker->GetAttack(), attacker->IsPoisonous());

		if (attacked.IsMinion()) {
			// If minion, deal damage equal to the attacked's attack
			StageHelper::DealDamage(attacker, attacked->GetAttack(), attacked->IsPoisonous());
		}

		attacker->AttackedOnce();

		if (attacker.IsMinion() && attacker->IsFreezeAttacker()) attacked->SetFreezed(true);
		if (attacked.IsMinion() && attacked->IsFreezeAttacker()) attacker->SetFreezed(true);
	}

	inline void StageHelper::RemoveMinionsIfDead(Board & board, SlotIndex side)
	{
		std::list<std::function<void()>> death_triggers;

		while (true) { // loop until no deathrattle are triggered
			death_triggers.clear();

			// mark as pending death
			for (auto it = board.object_manager.GetMinionInserterAtBeginOfSide(board, side); !it.IsEnd(); it.GoToNext())
			{
				auto manipulator = it.ConverToManipulator();
				if (!it.IsPendingRemoval() && manipulator.GetHP() > 0) continue;

				it.MarkPendingRemoval();

				for (auto const& trigger : manipulator.GetAndClearOnDeathTriggers()) {
					death_triggers.push_back(std::bind(trigger, it.ConverToManipulator()));
				}
			}

			// trigger deathrattles
			for (auto const& death_trigger : death_triggers) {
				death_trigger();
			}

			// actually remove died minions
			for (auto it = board.object_manager.GetMinionInserterAtBeginOfSide(board, side); !it.IsEnd();)
			{
				if (!it.IsPendingRemoval()) {
					it.GoToNext();
					continue;
				}

				// remove died minion
				auto manipulator = it.ConverToManipulator();

				// remove all effects (including auras)
				manipulator.ClearEnchantments();
				manipulator.ClearAuras();

				it.EraseAndGoToNext();
			}

			if (death_triggers.empty()) break;
		}
	}

	inline bool StageHelper::CheckHeroMinionDead(Board & board)
	{
		if (board.object_manager.GetPlayerHero(board)->GetHP() <= 0) {
			board.stage = STAGE_LOSS;
			return true;
		}

		if (board.object_manager.GetOpponentHero(board)->GetHP() <= 0) {
			board.stage = STAGE_WIN;
			return true;
		}

		RemoveMinionsIfDead(board, SLOT_PLAYER_SIDE);
		RemoveMinionsIfDead(board, SLOT_OPPONENT_SIDE);

		return false; // state not changed
	}

	// return true if stage changedW
	inline bool StageHelper::PlayMinion(Board & board, Card const & card, SlotIndex playing_side, PlayMinionData const & data)
	{
		Cards::CardCallbackManager::BattleCry(card.id, board, playing_side, data);
		if (StageHelper::CheckHeroMinionDead(board)) return true;

		auto inserter = board.object_manager.GetMinionInserter(board, data.put_location);

		StageHelper::SummonMinion(card, inserter);

		return false;
	}

	// return true if stage changedW
	inline bool StageHelper::SummonMinion(Card const & card, BoardObjects::MinionInserter & inserter)
	{
		BoardObjects::Minion summoning_minion;
		summoning_minion.Summon(card);

		if (inserter.GetMinions().IsFull()) return false;

		// add minion
		auto summoned_minion = inserter.InsertBefore(std::move(summoning_minion));

		Cards::CardCallbackManager::AfterSummoned(card.id, summoned_minion);

		return true;
	}

	inline bool StageHelper::EquipWeapon(Board & board, Card const & card, SlotIndex playing_side, Move::EquipWeaponData const & data)
	{
		auto & playing_hero = board.object_manager.GetHeroBySide(board, playing_side);

		playing_hero.DestroyWeapon();

		Cards::CardCallbackManager::BattleCry_Weapon(card.id, board, playing_side, data);
		if (StageHelper::CheckHeroMinionDead(board)) return true;

		playing_hero.EquipWeapon(card);

		return false;
	}

	inline void StageHelper::Fatigue(GameEngine::Board & board, SlotIndex side)
	{
		if (SlotIndexHelper::IsPlayerSide(side)) {
			++board.player_stat.fatigue_damage;
			StageHelper::DealDamage(board.object_manager.GetPlayerHero(board), board.player_stat.fatigue_damage, false);
		}
		else {
			++board.opponent_stat.fatigue_damage;
			StageHelper::DealDamage(board.object_manager.GetOpponentHero(board), board.opponent_stat.fatigue_damage, false);
		}
	}
}