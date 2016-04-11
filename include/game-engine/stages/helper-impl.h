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

			case Card::TYPE_SPELL:
				GetBoardMoves_PlaySpell(player, hand_idx, playing_card, next_moves);
				break;

			default:
				throw std::runtime_error("unknown hand card type");
				break;
			}
		}

		// the choices to attack by hero/minion
		GetBoardMoves_Attack(player, next_moves);

		// the choice to use hero power
		GetBoardMoves_HeroPower(player, next_moves);

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
			player, hand_card, SlotIndexHelper::GetMinionIndex(player.side, 0), required_targets));
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

		next_move_getter.AddItem(NextMoveGetter::ItemPlayHandCard(player, hand_card, Move::ACTION_PLAY_HAND_WEAPON, required_targets));
	}

	inline void StageHelper::GetBoardMoves_PlaySpell(Player const& player, Hand::Locator hand_card, Card const& playing_card, NextMoveGetter &next_move_getter)
	{
		if (player.stat.crystal.GetCurrent() < playing_card.cost) return;

		SlotIndexBitmap required_targets;
		bool meet_requirements;
		if (Cards::CardCallbackManager::GetRequiredTargets(playing_card.id, player, required_targets, meet_requirements) &&
			meet_requirements == false)
		{
			return;
		}

		next_move_getter.AddItem(NextMoveGetter::ItemPlayHandCard(player, hand_card, Move::ACTION_PLAY_HAND_SPELL, required_targets));
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

	inline void StageHelper::GetBoardMoves_HeroPower(Player const & player, NextMoveGetter & next_move_getter)
	{
		if (player.stat.crystal.GetCurrent() < player.hero.GetHeroPower().cost) return;
		if (player.hero.GetHeroPower().used_this_turn) return;

		SlotIndexBitmap required_targets;
		bool meet_requirements;
		if (Cards::CardCallbackManager::GetRequiredTargets(player.hero.GetHeroPower().card_id, player, required_targets, meet_requirements) &&
			meet_requirements == false)
		{
			return;
		}

		next_move_getter.AddItem(NextMoveGetter::ItemUseHeroPower(required_targets));
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
		constexpr int weight_play_spell = 100;
		constexpr int weight_attack = 100;
		constexpr int weight_hero_power = 100;

		auto & board = player.board;

		moves.Clear();

		// the choice to end turn
		move.action = Move::ACTION_END_TURN;
		moves.AddMove(move, weight_end_turn);

		// the choice to use hero power
		if (player.stat.crystal.GetCurrent() >= player.hero.GetHeroPower().cost && !player.hero.GetHeroPower().used_this_turn) {
			SlotIndexBitmap hero_power_required_targets;
			bool hero_power_meet_requirements;
			if (!Cards::CardCallbackManager::GetRequiredTargets(player.hero.GetHeroPower().card_id, player, hero_power_required_targets, hero_power_meet_requirements)
				|| hero_power_meet_requirements)
			{
				move.action = Move::ACTION_HERO_POWER;
				if (hero_power_required_targets.None()) move.data.use_hero_power_data.target = SLOT_INVALID;
				else move.data.use_hero_power_data.target = GetOneRandomTarget(board, hero_power_required_targets);

				moves.AddMove(move, weight_hero_power);
			}
		}

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
				move.data.play_hand_minion_data.data.put_location = SlotIndexHelper::GetMinionIndex(player.side, 0);
				if (required_targets.None()) move.data.play_hand_minion_data.data.target = SLOT_INVALID;
				else move.data.play_hand_minion_data.data.target = GetOneRandomTarget(board, required_targets);

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
				move.data.play_hand_card_data.hand_card = hand_idx;
				move.data.play_hand_card_data.card_id = playing_card.id;
				if (required_targets.None()) move.data.play_hand_card_data.target = SLOT_INVALID;
				else move.data.play_hand_card_data.target = GetOneRandomTarget(board, required_targets);

				moves.AddMove(move, weight_play_minion);
				break;

			case Card::TYPE_SPELL:
				if (player.stat.crystal.GetCurrent() < playing_card.cost) continue;

				if (Cards::CardCallbackManager::GetRequiredTargets(playing_card.id, player, required_targets, meet_requirements)
					&& meet_requirements == false)
				{
					break;
				}

				move.action = Move::ACTION_PLAY_HAND_SPELL;
				move.data.play_hand_card_data.hand_card = hand_idx;
				move.data.play_hand_card_data.card_id = playing_card.id;
				if (required_targets.None()) move.data.play_hand_card_data.target = SLOT_INVALID;
				else move.data.play_hand_card_data.target = GetOneRandomTarget(board, required_targets);

				moves.AddMove(move, weight_play_spell);
				break;

			default:
				throw std::runtime_error("unknown hand card type");
			}
		}

		// the choices to attack by hero/minion
		SlotIndexBitmap attacker;
		SlotIndexBitmap attacked;

		attacker = SlotIndexHelper::GetTargets(player.side, SlotIndexHelper::TARGET_TYPE_ATTACKABLE, player.board);

		if (!attacker.None()) {
			attacked = SlotIndexHelper::GetTargets(player.opposite_side, SlotIndexHelper::TARGET_TYPE_CAN_BE_ATTACKED, player.board);

			while (!attacker.None()) {
				SlotIndex attacker_idx = attacker.GetOneTarget(); // always choose the first attacker is reasonable, since all attackable minion will eventually got a chance to attack
				attacker.ClearOneTarget(attacker_idx);

				while (!attacked.None()) {
					SlotIndex attacked_idx = GetOneRandomTarget(board, attacked);
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

	inline void StageHelper::DealDamage(BoardTargets const & targets, int damage, bool poisonous)
	{
		// We need to get all minions at once, since a minion might summon another minion when the damage is dealt
		// And by the game rule, the newly-summoned minion should not get damaged
		// --> all the minions iterators are stored in 'targets'
		for (auto & minion : targets.GetMinionIterators()) {
			StageHelper::DealDamage(*minion, damage, poisonous);
		}
		if (targets.HasPlayerHero()) StageHelper::DealDamage(targets.GetBoard().player.hero, damage, poisonous);
		if (targets.HasOpponentHero()) StageHelper::DealDamage(targets.GetBoard().opponent.hero, damage, poisonous);
	}

	inline void StageHelper::DealDamageToOneRandomValidTarget(BoardTargets const & targets, int damage, bool poisonous)
	{
		try {
			BoardObject obj = targets.GetOneRandomValidTarget();
			StageHelper::DealDamage(obj, damage, poisonous);
		}
		catch (std::out_of_range oor)
		{
			// no valid target
			return;
		}
	}

	inline void StageHelper::Transform(Board & board, SlotIndex target, int new_card_id)
	{
		auto & minion = board.object_manager.GetMinion(target);

		board.hook_manager.HookBeforeMinionTransform(minion, new_card_id);

		// do not trigger deathrattle
		// do not trigger on-summon triggers
		minion.enchantments.Clear();
		minion.auras.Clear();

		// deathrattle will be added in the MinionData::FromCard()
		minion.Transform(MinionData::FromCard(CardDatabase::GetInstance().GetCard(new_card_id)));

		board.hook_manager.HookAfterMinionTransformed(minion);
	}

	inline void StageHelper::Heal(GameEngine::Board & board, SlotIndex target_idx, int heal_amount)
	{
		if (target_idx == SlotIndex::SLOT_PLAYER_HERO) {
			board.player.hero.Heal(heal_amount);
		}
		else if (target_idx == SlotIndex::SLOT_OPPONENT_HERO) {
			board.opponent.hero.Heal(heal_amount);
		}
		else {
			board.object_manager.GetMinion(target_idx).Heal(heal_amount);
		}
	}

	inline Minion & StageHelper::RandomChooseMinion(Minions & minions)
	{
		int count = minions.GetMinionCount();
		if (count == 0) throw std::runtime_error("invalid argument");

		const int rand = minions.GetBoard().random_generator.GetRandom(count);
		return minions.GetMinion(rand);
	}

	inline SlotIndex StageHelper::GetOneRandomTarget(GameEngine::Board & board, SlotIndexBitmap targets)
	{
		int rand = board.random_generator.GetRandom(targets.Count());
		for (int i = 0; i < rand; ++i) {
			targets.ClearOneTarget(targets.GetOneTarget());
		}
		return targets.GetOneTarget();
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

	inline bool StageHelper::HandleAttack(GameEngine::Board & board, GameEngine::SlotIndex attacker_idx, GameEngine::SlotIndex attacked_idx)
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

		if (StageHelper::CheckHeroMinionDead(board)) return true; // game ends;
		return false;
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
				it->enchantments.Clear();
				it->auras.Clear();

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

	inline bool StageHelper::PlayMinion(Player & player, Board::PlayHandMinionData & data)
	{
		Board & board = player.board;

		auto playing_card = player.hand.GetCard(data.hand_card);
		player.hand.RemoveCard(data.hand_card);

#ifdef DEBUG
		if (playing_card.type != Card::TYPE_MINION) throw std::runtime_error("card type is not minion");
#endif

		player.stat.crystal.CostCrystals(playing_card.cost);

#ifndef CHOOSE_WHERE_TO_PUT_MINION
		if (data.data.put_location != SlotIndexHelper::GetMinionIndex(player.side, 0)) throw std::runtime_error("check failed");
		if (player.minions.GetMinionCount() >= 7) throw std::runtime_error("minions full");
		data.data.put_location = SlotIndexHelper::GetMinionIndex(player.side, board.random_generator.GetRandom(player.minions.GetMinionCount() + 1));
#endif

		return StageHelper::PlayMinion(player, playing_card, data.data);
	}

	// return true if game ends
	inline bool StageHelper::PlayMinion(Player & player, Card const & card, PlayMinionData const & data)
	{
#ifdef DEBUG
		if (card.type != Card::TYPE_MINION) throw std::runtime_error("card type is not minion");
#endif

		Cards::CardCallbackManager::BattleCry(card.id, player, data);
		if (StageHelper::CheckHeroMinionDead(player.board)) return true;

		auto it = player.board.object_manager.GetMinionIterator(data.put_location);

		StageHelper::SummonMinion(card, it);
		if (StageHelper::CheckHeroMinionDead(player.board)) return true;

		return false;
	}

	// return true if stage changed
	inline bool StageHelper::SummonMinion(Card const & card, MinionIterator & it)
	{
		if (it.GetMinions().IsFull()) return false;

		// add minion
		auto summoned_minion = it.GetMinions().InsertBefore(it, MinionData::FromCard(card));

		// attach aura
		Cards::CardCallbackManager::AttachAura(card.id, summoned_minion->auras);

		Cards::CardCallbackManager::AfterSummoned(card.id, summoned_minion);

		return true;
	}

	inline bool StageHelper::EquipWeapon(Player & player, Board::PlayHandCardData const & data)
	{
		auto playing_card = player.hand.GetCard(data.hand_card);
		player.hand.RemoveCard(data.hand_card);

#ifdef DEBUG
		if (playing_card.type != Card::TYPE_WEAPON) throw std::runtime_error("card type is not weapon");
		if (playing_card.id != data.card_id) throw std::runtime_error("card id not match");
#endif

		player.stat.crystal.CostCrystals(playing_card.cost);

		player.hero.DestroyWeapon();

		Cards::CardCallbackManager::Weapon_BattleCry(playing_card.id, player, data.target);
		if (StageHelper::CheckHeroMinionDead(player.board)) return true;

		player.hero.EquipWeapon(playing_card);
		Cards::CardCallbackManager::Weapon_AfterEquipped(playing_card.id, player.hero);
		if (StageHelper::CheckHeroMinionDead(player.board)) return true;

		auto deathrattle = Cards::CardCallbackManager::GetWeaponDeathrattle(playing_card.id);
		if (deathrattle) {
			player.hero.AddWeaponOnDeathTrigger(Hero::WeaponOnDeathTrigger(deathrattle));
		}

		return false;
	}

	inline bool StageHelper::PlaySpell(Player & player, Board::PlayHandCardData const & data)
	{
		auto playing_card = player.hand.GetCard(data.hand_card);
		player.hand.RemoveCard(data.hand_card);

#ifdef DEBUG
		if (playing_card.type != Card::TYPE_SPELL) throw std::runtime_error("card type is not spell");
		if (playing_card.id != data.card_id) throw std::runtime_error("card id not match");
#endif

		player.stat.crystal.CostCrystals(playing_card.cost);

		Cards::CardCallbackManager::Spell_Go(playing_card.id, player, data.target);
		if (StageHelper::CheckHeroMinionDead(player.board)) return true;

		return false;
	}

	inline bool StageHelper::UseHeroPower(Player & player, SlotIndex target)
	{
		player.stat.crystal.CostCrystals(player.hero.GetHeroPower().cost);

		Cards::CardCallbackManager::HeroPower_Go(player.hero.GetHeroPower().card_id, player, target);
		player.hero.UsedHeroPowerOnce();

		if (StageHelper::CheckHeroMinionDead(player.board)) return true;

		return false;
	}

	inline void StageHelper::Fatigue(GameEngine::Board & board, SlotIndex side)
	{
		if (SlotIndexHelper::IsPlayerSide(side)) {
			++board.player.stat.fatigue_damage;
			StageHelper::DealDamage(board.player.hero, board.player.stat.fatigue_damage, false);
		}
		else {
			++board.opponent.stat.fatigue_damage;
			StageHelper::DealDamage(board.opponent.hero, board.opponent.stat.fatigue_damage, false);
		}
	}
}