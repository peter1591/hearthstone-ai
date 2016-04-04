#pragma once

DEFINE_CARD_CLASS_START(EX1_584)

// Ancient Mage
static void AfterSummoned(MinionIterator summoned_minion)
{
	typedef Enchantment_BuffMinion_C<0, 0, 1, 0, false> EnchantType;

	if (summoned_minion.IsBegin() == false) {
		auto prev = summoned_minion;
		prev.GoToPrevious()->enchantments.Add(std::make_unique<EnchantType>(), nullptr);
	}

	auto next = summoned_minion;
	next.GoToNext();
	if (next.IsEnd() == false) next->enchantments.Add(std::make_unique<EnchantType>(), nullptr);
}

DEFINE_CARD_CLASS_END()
