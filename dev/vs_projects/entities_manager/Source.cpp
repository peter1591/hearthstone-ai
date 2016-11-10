#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>
#include "EntitiesManager/EntitiesManager.h"

#include "Manipulators/Helpers/EnchantmentHelper.h"

#include "Enchantment/AddAttack.h"
#include "Enchantment/AddAttack_Tier2.h"
#include "Enchantment/AddAttack_Aura.h"

static void test1()
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
}

template <typename EnchantmentType_>
class AuraHelper
{
public:
	typedef EnchantmentType_ EnchantmentType;

	AuraHelper(CardRef eligible1, CardRef eligible2) : eligible1_(eligible1), eligible2_(eligible2) {}

	template <typename T>
	bool IsEligible(T&& target)
	{
		static_assert(std::is_same<std::decay_t<T>, CardRef>::value, "Wrong type");

		if (target == eligible1_) return true;
		if (target == eligible2_) return true;
		return false;
	}

	template <typename Functor>
	void IterateEligibles(Functor&& op)
	{
		op(eligible2_);
		op(eligible1_);
	}

	template <typename T>
	std::unique_ptr<Enchantment::Base> CreateEnchantmentFor(T&& target)
	{
		static_assert(std::is_same<std::decay_t<T>, CardRef>::value, "Wrong type");

		return std::unique_ptr<Enchantment::Base>(new EnchantmentType());
	}

private:
	CardRef eligible1_;
	CardRef eligible2_;
};

static void test2()
{
	EntitiesManager mgr;

	Entity::RawCard c1;
	c1.card_type = Entity::kCardTypeMinion;
	c1.card_id = "card_id_1";
	c1.enchanted_states.zone = Entity::kCardZoneDeck;
	c1.enchanted_states.cost = 5;
	CardRef r1 = mgr.PushBack(Entity::Card(c1));

	Entity::RawCard c2;
	c2.card_type = Entity::kCardTypeMinion;
	c2.card_id = "card_id_2";
	c2.enchanted_states.zone = Entity::kCardZoneDeck;
	c2.enchanted_states.cost = 5;
	CardRef r2 = mgr.PushBack(Entity::Card(c2));

	Entity::RawCard c3;
	c3.card_type = Entity::kCardTypeMinion;
	c3.card_id = "card_id_3";
	c3.enchanted_states.zone = Entity::kCardZoneDeck;
	c3.enchanted_states.cost = 9;
	CardRef r3 = mgr.PushBack(Entity::Card(c3));

	typedef AuraHelper<Enchantment::AddAttack_Aura> ClientAuraHelper;
	ClientAuraHelper client_aura_helper(r1, r2);
	mgr.GetMinionManipulator(r3).GetAuraHelper().Update(client_aura_helper);

	mgr.GetMinionManipulator(r3).GetAuraHelper().Update(client_aura_helper);

	auto mgr2 = mgr;

	ClientAuraHelper client_aura_helper2(r1, r3);
	mgr2.GetMinionManipulator(r3).GetAuraHelper().Update(client_aura_helper2);
}

int main()
{
	//test1();
	test2();

	return 0;
}