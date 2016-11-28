#pragma once

DEFINE_CARD_CLASS_START(CS2_045)

// Rockbiter Weapon

static void GetRequiredTargets(Player const& player, SlotIndexBitmap &targets, bool & meet_requirements)
{
	targets = SlotIndexHelper::GetTargets(player, SlotIndexHelper::TARGET_SPELL_FRIENDLY_CHARACTERS);
	meet_requirements = !targets.None();
}

static void Spell_Go(Player &player, SlotIndex target)
{
	if (SlotIndexHelper::IsHero(target)) {
		player.board.object_manager.GetPlayer(target).enchantments.Add(
			std::make_unique<Enchantment_BuffPlayer_C<3, 0, true>>());
	}
	else {
		player.board.object_manager.GetMinion(target).enchantments.Add(
			std::make_unique<Enchantment_BuffMinion_C<3, 0, 0, 0, true>>());
	}
}

DEFINE_CARD_CLASS_END()