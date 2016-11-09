#include "Enchantments/TieredEnchantments.h"

#include "Enchantment/AddAttack.h"
#include "Enchantment/AddAttack_Tier2.h"
#include "Enchantment/AddAttack_Aura.h"


int main(void)
{
	TieredEnchantments mgr;

	mgr.PushBack<Enchantment::AddAttack>(std::unique_ptr<Enchantment::Base>(new Enchantment::AddAttack()));
	mgr.PushBack<Enchantment::AddAttack_Tier2>(std::unique_ptr<Enchantment::Base>(new Enchantment::AddAttack_Tier2()));
	auto t1 = mgr.PushBack<Enchantment::AddAttack_Aura>(std::unique_ptr<Enchantment::Base>(new Enchantment::AddAttack_Aura()));

	return 0;
}