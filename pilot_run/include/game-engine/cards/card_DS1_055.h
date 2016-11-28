#pragma once

DEFINE_CARD_CLASS_START(DS1_055)

// Darkscale Healer

static void BattleCry(Player & player, Move::PlayMinionData const& play_minion_data)
{
	constexpr int heal_amount = 2;

	// TODO: Refactor out to be a convenient function?
	player.hero.Heal(heal_amount);
	for (auto it = player.minions.GetIterator(0); !it.IsEnd(); it.GoToNext()) {
		it->Heal(heal_amount);
	}
}

DEFINE_CARD_CLASS_END()