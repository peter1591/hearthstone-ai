#ifndef GAME_ENGINE_CARDS_CARD_FP1_021
#define GAME_ENGINE_CARDS_CARD_FP1_021

DEFINE_CARD_CLASS_START(FP1_021)
// Weapon: Death's Bite
			
static void WeaponDeathrattle(GameEngine::Hero & equipped_hero)
{
	auto & board = equipped_hero.GetBoard();
	constexpr bool damage_poisonous = false;
	constexpr int damage = 1;

	// We need to get all minions at once, since a minion might summon another minion when the damage is dealt
	// And by the game rule, the newly-summoned minion should not get damaged
	std::list<GameEngine::MinionIterator> all_minions;
	for (auto it = board.player.minions.GetIterator(0); !it.IsEnd(); it.GoToNext()) {
		all_minions.push_back(it);
	}
	for (auto it = board.opponent.minions.GetIterator(0); !it.IsEnd(); it.GoToNext()) {
		all_minions.push_back(it);
	}

	for (auto & target : all_minions) {
		StageHelper::DealDamage(GameEngine::BoardObject(*target), damage, damage_poisonous);
	}
}

static void Weapon_AfterEquipped(GameEngine::Hero & equipped_hero)
{
	equipped_hero.AddWeaponOnDeathTrigger(GameEngine::Hero::WeaponOnDeathTrigger(WeaponDeathrattle));
}
DEFINE_CARD_CLASS_END()
#endif
