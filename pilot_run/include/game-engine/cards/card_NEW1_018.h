#ifndef GAME_ENGINE_CARDS_CARD_NEW1_018
#define GAME_ENGINE_CARDS_CARD_NEW1_018

DEFINE_CARD_CLASS_START(NEW1_018)
// Bloodsail Raider

static void AfterSummoned(MinionIterator summoned_minion)
{
	Hero * playing_hero = nullptr;

	if (summoned_minion->IsPlayerSide()) playing_hero = &summoned_minion.GetBoard().player.hero;
	else playing_hero = &summoned_minion.GetBoard().opponent.hero;

	const int attack_boost = playing_hero->GetWeaponAttack();

	if (attack_boost > 0) {
		auto enchant = std::make_unique<Enchantment_BuffMinion>(attack_boost, 0, 0, 0, false);
		summoned_minion->enchantments.Add(std::move(enchant));
	}
}
DEFINE_CARD_CLASS_END()

#endif
