#pragma once

DEFINE_CARD_CLASS_START(CS2_226)

// Frostwolf Warlord

static void AfterSummoned(MinionIterator summoned_minion)
{
	int other_friendly_minions_count = summoned_minion->GetMinions().GetMinionCount() - 1;

	if (other_friendly_minions_count < 0) throw std::runtime_error("logic error");
	if (other_friendly_minions_count == 0) return;

	int attack_boost = other_friendly_minions_count;
	int hp_boost = other_friendly_minions_count;

	summoned_minion->enchantments.Add(std::make_unique<Enchantment_BuffMinion>(
		attack_boost, hp_boost, 0, 0, false));
}

DEFINE_CARD_CLASS_END()