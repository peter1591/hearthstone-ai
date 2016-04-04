#pragma once

DEFINE_CARD_CLASS_START(CS2_025)

// Arcane Explosion

static void Spell_Go(Player &player, SlotIndex)
{
	constexpr bool damage_poisonous = false;
	int damage = 1 + player.GetTotalSpellDamage();

	// We need to get all minions at once, since a minion might summon another minion when the damage is dealt
	// And by the game rule, the newly-summoned minion should not get damaged
	std::list<MinionIterator> all_minions;
	for (auto it = player.GetOppositePlayer().minions.GetIterator(0); !it.IsEnd(); it.GoToNext()) {
		all_minions.push_back(it);
	}

	for (auto & minion : all_minions) {
		StageHelper::DealDamage(*minion, damage, damage_poisonous);
	}
}

DEFINE_CARD_CLASS_END()
