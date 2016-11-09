#include <cassert>
#include <iostream>
#include "EntitiesManager/EntitiesManager.h"

#include "Manipulators/Helpers/EnchantmentHelper.h"

#include "Enchantment/AddAttack.h"
#include "Enchantment/AddAttack_Tier2.h"
#include "Enchantment/AddAttack_Aura.h"

int main(void)
{
	EntitiesManager mgr;

	Entity::RawCard c1;
	c1.card_type = Entity::kCardTypeMinion;
	c1.card_id = "card_id_1";
	c1.enchanted_states.zone = Entity::kCardZoneDeck;
	c1.enchanted_states.cost = 5;
	CardRef r1 = mgr.PushBack(Entity::Card(c1));
	assert(mgr.Get(r1).GetZone() == Entity::kCardZoneDeck);

	c1.enchanted_states.zone = Entity::kCardZoneGraveyard;
	assert(mgr.Get(r1).GetZone() == Entity::kCardZoneDeck);

	mgr.GetMinionManipulator(r1).ChangeZone(Entity::kCardZoneHand);
	assert(mgr.Get(r1).GetZone() == Entity::kCardZoneHand);

	auto mgr2(mgr);
	assert(mgr.Get(r1).GetZone() == Entity::kCardZoneHand);
	assert(mgr2.Get(r1).GetZone() == Entity::kCardZoneHand);

	mgr2.GetMinionManipulator(r1).ChangeZone(Entity::kCardZonePlay);
	assert(mgr.Get(r1).GetZone() == Entity::kCardZoneHand);
	assert(mgr2.Get(r1).GetZone() == Entity::kCardZonePlay);

	mgr2.GetMinionManipulator(r1).SetCost(9);
	assert(mgr.Get(r1).GetCost() == 5);
	assert(mgr2.Get(r1).GetCost() == 9);
	
	auto manipulator = mgr.GetMinionManipulator(r1);
	auto ref1 = manipulator.GetEnchantmentHelper().CreateAndAdd<Enchantment::AddAttack>();
	auto ref2 = manipulator.GetEnchantmentHelper().CreateAndAdd<Enchantment::AddAttack_Tier2>();
	auto ref3 = manipulator.GetEnchantmentHelper().CreateAndAdd<Enchantment::AddAttack>();
	auto ref4 = manipulator.GetEnchantmentHelper().CreateAndAdd<Enchantment::AddAttack_Tier2>();

	auto ref5 = manipulator.GetEnchantmentHelper().CreateAndAdd<Enchantment::AddAttack_Aura>();
	auto ref6 = manipulator.GetEnchantmentHelper().CreateAndAdd<Enchantment::AddAttack_Aura>();
	auto ref7 = manipulator.GetEnchantmentHelper().CreateAndAdd<Enchantment::AddAttack_Aura>();

	auto mgr3 = mgr;

	mgr3.GetMinionManipulator(r1).GetEnchantmentHelper().Remove<Enchantment::AddAttack_Aura>(ref6);



	return 0;
}