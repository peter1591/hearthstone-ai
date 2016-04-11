#pragma once

#error "This file should not be included"

DEFINE_CARD_CLASS_START(CS2_999999)

static void GetRequiredTargets(Player const& equipping_player, SlotIndexBitmap &targets, bool & meet_requirements)
{
}

static void Weapon_BattleCry(Player &equipping_player, SlotIndex target)
{
}

static void Weapon_AfterEquipped(Hero & equipped_hero)
{
	// Attach deathrattle here (if any); the framework will not automatically attach it.
	// TODO: attach automatically
}

static void WeaponDeathrattle(Hero & equipped_hero)
{

}

DEFINE_CARD_CLASS_END()