#pragma once

DEFINE_CARD_CLASS_START(GVG_059)
// Coghammer
static void Weapon_BattleCry(Player &equipping_player, Move::EquipWeaponData const&)
{
	const int minion_count = equipping_player.minions.GetMinionCount();
	if (minion_count == 0) return; // no minion to buff

	const int r = equipping_player.board.random_generator.GetRandom(minion_count);

	auto & buff_target = equipping_player.minions.GetMinion(r);
	constexpr int buff_flags =
		(1 << MinionStat::FLAG_SHIELD) |
		(1 << MinionStat::FLAG_TAUNT);
	auto enchant = std::make_unique<Enchantment_BuffMinion_C<0, 0, 0, buff_flags, false>>();
	buff_target.enchantments.Add(std::move(enchant), nullptr);
}
DEFINE_CARD_CLASS_END()
