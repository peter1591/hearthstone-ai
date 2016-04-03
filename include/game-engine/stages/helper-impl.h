#pragma once

#include "game-engine/stages/helper.h"
#include "game-engine/cards/common.h"

namespace GameEngine
{
	inline bool StageHelper::PlayerDrawCard(Player & player)
	{
		auto & board = player.board;

		if (player.hand.HasCardToDraw()) {
			StageHelper::Fatigue(board, player.side);
			return StageHelper::CheckHeroMinionDead(board);
		}

		if (player.hand.GetCount() < 10) {
			player.hand.DrawOneCardToHand();
		}
		else {
			// hand can have maximum of 10 cards
			player.hand.DrawOneCardAndDiscard();
			// TODO: distroy card (trigger deathrattle?)
		}

		return false;
	}

	inline void StageHelper::GetBoardMoves(Player const& player, NextMoveGetter & next_moves, bool & all_cards_determined)
	{
		bool const minions_full = player.minions.IsFull();

		// if all the hand cards are determined,
		// then all identical boards have the same next moves
		// --> is_deterministic is true
		// if some of the hand cards are not yet determined
		// then some different boards might considered as the identical boards in MCTS tree
		// and those different boards might produce different set of next moves
		// --> is_deterministic is false
		all_cards_determined = true;

		for (Hand::Locator hand_idx = 0; hand_idx < player.hand.GetCount(); ++hand_idx)
		{
			bool hand_card_determined = false;
			const Card &playing_card = player.hand.GetCard(hand_idx, hand_card_determined);
			if (!hand_card_determined) all_cards_determined = false;

			switch (playing_card.type) {
			case Card::TYPE_MINION:
				if (minions_full) continue;
				GetBoardMoves_PlayMinion(player, hand_idx, playing_card, next_moves);
				break;

			case Card::TYPE_WEAPON:
				GetBoardMoves_EquipWeapon(player, hand_idx, playing_card, next_moves);
				break;

			default:
				break;
			}
		}

		// the choices to attack by hero/minion
		GetBoardMoves_Attack(player, next_moves);

		// the choice to end turn
		Move move_end_turn;
		move_end_turn.action = Move::ACTION_END_TURN;
		next_moves.AddItem(std::move(move_end_turn));
	}

	inline void StageHelper::GetBoardMoves_PlayMinion(Player const& player, Hand::Locator hand_card, Card const& playing_card,
		NextMoveGetter &next_move_getter)
	{
		if (player.stat.crystal.GetCurrent() < playing_card.cost) return;

		SlotIndexBitmap required_targets;
		bool meet_requirements;
		if (Cards::CardCallbackManager::GetRequiredTargets(playing_card.id, player, required_targets, meet_requirements) &&
			meet_requirements == false)
		{
			return;
		}

#ifdef CHOOSE_WHERE_TO_PUT_MINION
		for (int i = 0; i <= player.minions.GetMinionCount(); ++i)
		{
			SlotIndex idx = SlotIndexHelper::GetMinionIndex(player.side, i);
			next_move_getter.AddItem(NextMoveGetter::ItemPlayHandMinion(player, hand_card, idx, required_targets));
		}
#else
		next_move_getter.AddItem(NextMoveGetter::ItemPlayHandMinion(
			player, hand_card, SlotIndexHelper::GetPlayerMinionIndex(player.minions.GetMinionCount()), required_targets));
#endif
	}

	inline void StageHelper::GetBoardMoves_EquipWeapon(Player const& player, Hand::Locator hand_card, Card const& playing_card, NextMoveGetter &next_move_getter)
	{
		if (player.stat.crystal.GetCurrent() < playing_card.cost) return;

		SlotIndexBitmap required_targets;
		bool meet_requirements;
		if (Cards::CardCallbackManager::GetRequiredTargets(playing_card.id, player, required_targets, meet_requirements) &&
			meet_requirements == false)
		{
			return;
		}

		next_move_getter.AddItem(NextMoveGetter::ItemPlayHandWeapon(player, hand_card, required_targets));
	}

	inline void StageHelper::GetBoardMoves_Attack(Player const & player, NextMoveGetter & next_move_getter)
	{
		SlotIndexBitmap attacker;
		SlotIndexBitmap attacked;

		attacker = SlotIndexHelper::GetTargets(player.side, SlotIndexHelper::TARGET_TYPE_ATTACKABLE, player.board);

		if (!attacker.None()) {
			attacked = SlotIndexHelper::GetTargets(player.opposite_side, SlotIndexHelper::TARGET_TYPE_CAN_BE_ATTACKED, player.board);

			NextMoveGetter::ItemAttack player_attack_move(std::move(attacker), std::move(attacked));
			next_move_getter.AddItem(std::move(player_attack_move));
		}
	}

	inline void StageHelper::GetGoodBoardMove(unsigned int rand, Player const & player, Move &good_move)
	{
		// heuristic goes here
		// 1. play minion is good (always put minion to the rightmost)
		// 2. minion attack is good
		// 3. hero attack is good
		// 4. effective spell is good

		thread_local static std::unique_ptr<WeightedMoves> ptr_moves = nullptr;
		if (!ptr_moves) ptr_moves.reset(new WeightedMoves());

		WeightedMoves & moves = *ptr_moves; // do not re-allocate at each call
		Move move;

		constexpr int weight_end_turn = 1;
		constexpr int weight_play_minion = 100;
		constexpr int weight_equip_weapon = 100;
		constexpr int weight_attack = 100;

		moves.Clear();

		// the choice to end turn
		move.action = Move::ACTION_END_TURN;
		moves.AddMove(move, weight_end_turn);

		// the choices to play a card from hand
		bool can_play_minion = !player.minions.IsFull();
		SlotIndexBitmap required_targets;
		bool meet_requirements;
		for (size_t hand_idx = 0; hand_idx < player.hand.GetCount(); ++hand_idx)
		{
			Card const& playing_card = player.hand.GetCard(hand_idx);
			switch (playing_card.type) {
			case Card::TYPE_MINION:
				if (!can_play_minion) continue;
				if (player.stat.crystal.GetCurrent() < playing_card.cost) continue;

				if (Cards::CardCallbackManager::GetRequiredTargets(playing_card.id, player, required_targets, meet_requirements)
					&& meet_requirements == false)
				{
					break;
				}

				move.action = Move::ACTION_PLAY_HAND_MINION;
				move.data.play_hand_minion_data.hand_card = hand_idx;
				move.data.play_hand_minion_data.card_id = playing_card.id;
				move.data.play_hand_minion_data.data.put_location = SlotIndexHelper::GetMinionIndex(player.side, player.minions.GetMinionCount());
				if (required_targets.None()) move.data.play_hand_minion_data.data.target = SLOT_INVALID;
				else move.data.play_hand_minion_data.data.target = required_targets.GetOneTarget();

				moves.AddMove(move, weight_play_minion);
				break;

			case Card::TYPE_WEAPON:
				if (player.stat.crystal.GetCurrent() < playing_card.cost) continue;

				if (Cards::CardCallbackManager::GetRequiredTargets(playing_card.id, player, required_targets, meet_requirements)
					&& meet_requirements == false)
				{
					break;
				}

				move.action = Move::ACTION_PLAY_HAND_WEAPON;
				move.data.play_hand_weapon_data.hand_card = hand_idx;
				move.data.play_hand_weapon_data.card_id = playing_card.id;
				if (required_targets.None()) move.data.play_hand_weapon_data.data.target = SLOT_INVALID;
				else move.data.play_hand_weapon_data.data.target = required_targets.GetOneTarget();

				moves.AddMove(move, weight_play_minion);
				break;

			default:
				break; // TODO: handle other card types
			}
		}

		// the choices to attack by hero/minion
		SlotIndexBitmap attacker;
		SlotIndexBitmap attacked;

		attacker = SlotIndexHelper::GetTargets(player.side, SlotIndexHelper::TARGET_TYPE_ATTACKABLE, player.board);

		if (!attacker.None()) {
			attacked = SlotIndexHelper::GetTargets(player.opposite_side, SlotIndexHelper::TARGET_TYPE_CAN_BE_ATTACKED, player.board);

			while (!attacker.None()) {
				SlotIndex attacker_idx = attacker.GetOneTarget();
				attacker.ClearOneTarget(attacker_idx);

				while (!attacked.None()) {
					SlotIndex attacked_idx = attacked.GetOneTarget();
					attacked.ClearOneTarget(attacked_idx);

					move.action = Move::ACTION_ATTACK;
					move.data.attack_data.attacker_idx = attacker_idx;
					move.data.attack_data.attacked_idx = attacked_idx;
					moves.AddMove(move, weight_attack);
				} // for attacked
			} // for attacker
		}

		if (moves.Choose(rand, good_move) == false) {
			throw std::runtime_error("no action is available (should not happen)");
		}

		return;
	}

	inline void StageHelper::DealDamage(GameEngine::Board & board, SlotIndex taker_idx, int damage, bool poisonous)
	{
		return StageHelper::DealDamage(board.object_manager.GetObject(taker_idx), damage, poisonous);
	}

	inline void StageHelper::DealDamage(GameEngine::BoardObject taker, int damage, bool poisonous)
	{
		taker->TakeDamage(damage, poisonous);
	}

	inline void StageHelper::DealDamage(GameEngine::Hero & taker, int damage, bool poisonous)
	{
		taker.TakeDamage(damage, poisonous);
	}

	inline void StageHelper::DealDamage(GameEngine::Minion & taker, int damage, bool poisonous)
	{
		taker.TakeDamage(damage, poisonous);
	}

	inline SlotIndex StageHelper::GetTargetForForgetfulAttack(GameEngine::Board & board, SlotIndex origin_attacked)
	{
		const bool player_side = SlotIndexHelper::IsPlayerSide(origin_attacked);

		int possible_targets = 1 - 1; // +1 for the hero, -1 for the original attack target
		if (player_side) possible_targets += board.player.minions.GetMinionCount();
		else possible_targets += board.opponent.minions.GetMinionCount();

		if (possible_targets == 0)
		{
			// no other target available --> attack original target
			return origin_attacked;
		}

		if (board.random_generator.GetRandom(2) == 0) {
			// forgetful effect does not triggered
			return origin_attacked;
		}

		int r = board.random_generator.GetRandom(possible_targets);

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

		auto attacker = board.object_manager.GetObject(attacker_idx);

		for (int forgetful_count = attacker->GetForgetfulCount(); forgetful_count > 0; --forgetful_count) {
			attacked_idx = StageHelper::GetTargetForForgetfulAttack(board, attacked_idx);
		}

		auto attacked = board.object_manager.GetObject(attacked_idx);
		StageHelper::DealDamage(attacked, attacker->GetAttack(), attacker->IsPoisonous());

		if (attacked.IsMinion()) {
			// If minion, deal damage equal to the attacked's attack
			StageHelper::DealDamage(attacker, attacked->GetAttack(), attacked->IsPoisonous());
		}

		attacker->AttackedOnce();

		if (attacker.IsMinion() && attacker->IsFreezeAttacker()) attacked->SetFreezed();
		if (attacked.IsMinion() && attacked->IsFreezeAttacker()) attacker->SetFreezed();
	}

	inline void StageHelper::RemoveMinionsIfDead(Board & board, SlotIndex side)
	{
		std::list<std::function<void()>> death_triggers;

		while (true) { // loop until no deathrattle are triggered
			death_triggers.clear();

			// mark as pending death
			for (auto it = board.object_manager.GetMinionIteratorAtBeginOfSide(side); !it.IsEnd(); it.GoToNext())
			{
				if (!it->GetMinion().pending_removal && it->GetHP() > 0) continue;

				it.GetMinions().MarkPendingRemoval(it);

				for (auto const& trigger : it->GetAndClearOnDeathTriggers()) {
					std::function<void(MinionIterator&)> functor(trigger.func);
					death_triggers.push_back(std::bind(functor, it));
				}
			}

			// trigger deathrattles
			for (auto const& death_trigger : death_triggers) {
				death_trigger();
			}

			// actually remove dead minions
			for (auto it = board.object_manager.GetMinionIteratorAtBeginOfSide(side); !it.IsEnd();)
			{
				if (!it->GetMinion().pending_removal) {
					it.GoToNext();
					continue;
				}

				// remove all effects (including auras)
				it->ClearEnchantments();
				it->ClearHookListener();

				it.GetMinions().EraseAndGoToNext(it);
			}

			if (death_triggers.empty()) break;
		}
	}

	inline bool StageHelper::CheckHeroMinionDead(Board & board)
	{
		if (board.object_manager.GetObject(SLOT_PLAYER_HERO)->GetHP() <= 0) {
			board.stage = STAGE_LOSS;
			return true;
		}

		if (board.object_manager.GetObject(SLOT_OPPONENT_HERO)->GetHP() <= 0) {
			board.stage = STAGE_WIN;
			return true;
		}

		RemoveMinionsIfDead(board, SLOT_PLAYER_SIDE);
		RemoveMinionsIfDead(board, SLOT_OPPONENT_SIDE);

		return false; // state not changed
	}

	// return true if stage changed
	inline bool StageHelper::PlayMinion(Player & player, Card const & card, PlayMinionData const & data)
	{
		Cards::CardCallbackManager::BattleCry(card.id, player.board, player.side, data);
		if (StageHelper::CheckHeroMinionDead(player.board)) return true;

		auto it = player.board.object_manager.GetMinionIterator(data.put_location);

		StageHelper::SummonMinion(card, it);

		return false;
	}

	// return true if stage changed
	inline bool StageHelper::SummonMinion(Card const & card, MinionIterator & it)
	{
		MinionData summoning_minion;
		summoning_minion.Summon(card);

		if (it.GetMinions().IsFull()) return false;

		// add minion
		auto & summoned_minion = it.GetMinions().InsertBefore(it, std::move(summoning_minion));

		Cards::CardCallbackManager::AfterSummoned(card.id, summoned_minion);

		return true;
	}

	inline bool StageHelper::EquipWeapon(Player & player, Card const & card, Move::EquipWeaponData const & data)
	{
		player.hero.DestroyWeapon();

		Cards::CardCallbackManager::Weapon_BattleCry(card.id, player, data);
		if (StageHelper::CheckHeroMinionDead(player.board)) return true;

		player.hero.EquipWeapon(card);
		Cards::CardCallbackManager::Weapon_AfterEquipped(card.id, player.hero);
		if (StageHelper::CheckHeroMinionDead(player.board)) return true;

		return false;
	}

	inline void StageHelper::Fatigue(GameEngine::Board & board, SlotIndex side)
	{
		if (SlotIndexHelper::IsPlayerSide(side)) {
			++board.player.stat.fatigue_damage;
			StageHelper::DealDamage(board.object_manager.GetObject(SLOT_PLAYER_HERO), board.player.stat.fatigue_damage, false);
		}
		else {
			++board.opponent.stat.fatigue_damage;
			StageHelper::DealDamage(board.object_manager.GetObject(SLOT_OPPONENT_HERO), board.opponent.stat.fatigue_damage, false);
		}
	}
}