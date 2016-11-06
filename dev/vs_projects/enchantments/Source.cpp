#include "Enchantments/TieredEnchantments.h"

#include "Enchantment/AddAttack.h"
#include "Enchantment/AddAttack_Tier2.h"

int main(void)
{
	TieredEnchantments mgr;

	auto t1 = mgr.PushBack<Enchantment::AddAttack>(std::unique_ptr<Enchantment::Base>(new Enchantment::AddAttack()));
	auto t2 = mgr.PushBack<Enchantment::AddAttack_Tier2>(std::unique_ptr<Enchantment::Base>(new Enchantment::AddAttack_Tier2()));

	return 0;
}