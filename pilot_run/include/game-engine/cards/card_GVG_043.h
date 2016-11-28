#ifndef GAME_ENGINE_CARDS_CARD_GVG_043
#define GAME_ENGINE_CARDS_CARD_GVG_043

DEFINE_CARD_CLASS_START(GVG_043)
// Weapon: Glaivezooka

static void Weapon_BattleCry(Player & equipping_player, SlotIndex)
{
	constexpr int attack_boost = 1;

	const int minion_count = equipping_player.minions.GetMinionCount();
	if (minion_count == 0) return;

	StageHelper::RandomChooseMinion(equipping_player.minions).enchantments.Add(
		std::make_unique<Enchantment_BuffMinion_C<attack_boost, 0, 0, 0, false>>());
}
DEFINE_CARD_CLASS_END()

#endif
