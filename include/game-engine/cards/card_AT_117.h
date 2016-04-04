#ifndef GAME_ENGINE_CARDS_CARD_AT_117
#define GAME_ENGINE_CARDS_CARD_AT_117

DEFINE_CARD_CLASS_START(AT_117)
// Master of Ceremonies
static void AfterSummoned(GameEngine::MinionIterator summoned_minion)
{
	if (summoned_minion.GetMinions().GetPlayer().GetTotalSpellDamage() > 0) {
		summoned_minion->enchantments.Add(std::make_unique<Enchantment_BuffMinion_C<2, 2, 0, 0, false>>());
	}
}
DEFINE_CARD_CLASS_END()
#endif
