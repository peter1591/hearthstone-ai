#include <assert.h>
#include "State/Cards/Enchantments/TieredEnchantments.h"
#include "State/Cards/Card.h"

using namespace state::Cards;

struct Enchantment1
{
	static constexpr EnchantmentTiers tier = kEnchantmentTier2;

	Enchantments::ApplyFunctor apply_functor;
};

struct Enchantment2
{
	static constexpr EnchantmentTiers tier = kEnchantmentTier3;

	Enchantments::ApplyFunctor apply_functor;
};

int main(void)
{
	TieredEnchantments mgr;

	state::Cards::RawCard raw_card;
	state::Cards::Card card(raw_card);
	card.SetCost(0);

	Enchantment1 enchant1{ [](auto& states) {
		++states.cost;
	}};
	Enchantment1 enchant2{ [](auto& states) {
		states.cost *= 2;
	} };
	Enchantment2 enchant3{ [](auto& states) {
		states.cost += 3;
	} };

	auto r1 = mgr.PushBack(enchant1);
	assert(card.GetCost() == 0);
	mgr.ApplyAll(card.e);
	assert(card.GetCost() == 1);
	mgr.ApplyAll(card);
	assert(card.GetCost() == 2);

	auto mgr2 = mgr;
	assert(card.GetCost() == 2);
	mgr.ApplyAll(card);
	assert(card.GetCost() == 3);
	mgr2.ApplyAll(card);
	assert(card.GetCost() == 4);

	mgr.Remove<Enchantment1>(r1);
	assert(card.GetCost() == 4);
	mgr.ApplyAll(card);
	assert(card.GetCost() == 4);
	mgr2.ApplyAll(card);
	assert(card.GetCost() == 5);

	auto r3 = mgr.PushBack(enchant3);
	mgr.ApplyAll(card);
	assert(card.GetCost() == 8);

	auto r2 = mgr.PushBack(enchant2);
	mgr.ApplyAll(card);
	assert(card.GetCost() == 19);

	mgr.ApplyAll(card);
	assert(card.GetCost() == 41);

	mgr.Remove<Enchantment1>(r2);
	mgr.ApplyAll(card);
	assert(card.GetCost() == 44);

	return 0;
}