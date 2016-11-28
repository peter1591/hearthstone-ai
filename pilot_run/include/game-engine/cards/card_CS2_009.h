#pragma once

DEFINE_CARD_CLASS_START(CS2_009)

// Mark of the Wild

static void GetRequiredTargets(Player const& player, SlotIndexBitmap &targets, bool & meet_requirements)
{
	targets = SlotIndexHelper::GetTargets(player, SlotIndexHelper::TARGET_SPELL_ALL_MINIONS);
	meet_requirements = !targets.None();
}

static void Spell_Go(Player &player, SlotIndex target)
{
	auto & minion = player.board.object_manager.GetMinion(target);
	constexpr int buff_flags = 1 << MinionStat::FLAG_TAUNT;
	minion.enchantments.Add(std::make_unique<Enchantment_BuffMinion_C<2, 2, 0, buff_flags, false>>());
}

DEFINE_CARD_CLASS_END()