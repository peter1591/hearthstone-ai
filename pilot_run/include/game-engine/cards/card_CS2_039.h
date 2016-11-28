#pragma once

DEFINE_CARD_CLASS_START(CS2_039)

// Windfury

static void GetRequiredTargets(Player const& player, SlotIndexBitmap &targets, bool & meet_requirements)
{
	targets = SlotIndexHelper::GetTargets(player, SlotIndexHelper::TARGET_SPELL_ALL_MINIONS);
	meet_requirements = !targets.None();
}

static void Spell_Go(Player &player, SlotIndex target)
{
	constexpr int buff_stat = 1 << MinionStat::FLAG_WINDFURY;

	player.board.object_manager.GetMinion(target).enchantments.Add(
		std::make_unique<Enchantment_BuffMinion_C<0, 0, 0, buff_stat, false>>());
}

DEFINE_CARD_CLASS_END()