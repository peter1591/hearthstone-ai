#pragma once

DEFINE_CARD_CLASS_START(EX1_277)

// Arcane Missle

static void Spell_Go(Player &player, SlotIndex target)
{
	int times = 3 + player.GetTotalSpellDamage();

	for (int i = 0; i < times; ++i) {
		StageHelper::DealDamageToOneRandomValidTarget(BoardTargets::EnemyCharacters(player), 1, false);
	}
}

DEFINE_CARD_CLASS_END()