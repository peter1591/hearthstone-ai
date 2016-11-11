#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>
#include "State/State.h"

#include "Manipulators/Manipulators.h"

#include "Enchantment/AddAttack.h"
#include "Enchantment/AddAttack_Tier2.h"
#include "Enchantment/AddAttack_Aura.h"


static void test1()
{
	State::State state;

	Entity::RawCard c1;
	c1.card_type = Entity::kCardTypeMinion;
	c1.card_id = "card_id_1";
	c1.enchanted_states.player = State::kPlayerFirst;
	c1.enchanted_states.zone = Entity::kCardZoneDeck;
	c1.enchanted_states.cost = 5;
	CardRef r1 = state.mgr.PushBack(state, Entity::Card(c1));
	assert(state.mgr.Get(r1).GetZone() == Entity::kCardZoneDeck);

	c1.enchanted_states.zone = Entity::kCardZoneGraveyard;
	assert(state.mgr.Get(r1).GetZone() == Entity::kCardZoneDeck);

	state.mgr.GetMinionManipulator(r1).GetZoneChanger().Change(state, State::kPlayerFirst, Entity::kCardZoneHand, 0);
	assert(state.mgr.Get(r1).GetZone() == Entity::kCardZoneHand);

	auto state2 = state;
	assert(state.mgr.Get(r1).GetZone() == Entity::kCardZoneHand);
	assert(state2.mgr.Get(r1).GetZone() == Entity::kCardZoneHand);

	state2.mgr.GetMinionManipulator(r1).GetZoneChanger().Change(state2, State::kPlayerFirst, Entity::kCardZonePlay, 0);
	assert(state.mgr.Get(r1).GetZone() == Entity::kCardZoneHand);
	assert(state2.mgr.Get(r1).GetZone() == Entity::kCardZonePlay);

	state2.mgr.GetMinionManipulator(r1).SetCost(9);
	assert(state.mgr.Get(r1).GetCost() == 5);
	assert(state2.mgr.Get(r1).GetCost() == 9);

	assert(state2.mgr.Get(r1).GetZonePosition() == 0);

	CardRef r2 = state2.mgr.PushBack(state2, Entity::Card(c1));
	state2.mgr.GetMinionManipulator(r2).GetZoneChanger().Change(state2, State::kPlayerFirst, Entity::kCardZonePlay, 0);
	assert(state2.mgr.Get(r2).GetZonePosition() == 0);
	assert(state2.mgr.Get(r1).GetZonePosition() == 1);

	CardRef r3 = state2.mgr.PushBack(state2, Entity::Card(c1));
	state2.mgr.GetMinionManipulator(r3).GetZoneChanger().Change(state2, State::kPlayerFirst, Entity::kCardZonePlay, 2);
	assert(state2.mgr.Get(r2).GetZonePosition() == 0);
	assert(state2.mgr.Get(r1).GetZonePosition() == 1);
	assert(state2.mgr.Get(r3).GetZonePosition() == 2);

	CardRef r4 = state2.mgr.PushBack(state2, Entity::Card(c1));
	state2.mgr.GetMinionManipulator(r4).GetZoneChanger().Change(state2, State::kPlayerFirst, Entity::kCardZonePlay, 1);
	assert(state2.mgr.Get(r2).GetZonePosition() == 0);
	assert(state2.mgr.Get(r4).GetZonePosition() == 1);
	assert(state2.mgr.Get(r1).GetZonePosition() == 2);
	assert(state2.mgr.Get(r3).GetZonePosition() == 3);

	// steal minion
	state2.mgr.GetMinionManipulator(r1).GetZoneChanger().Change(state2, State::kPlayerSecond, Entity::kCardZonePlay, 0);
	assert(state2.mgr.Get(r1).GetZonePosition() == 0);

	assert(state2.mgr.Get(r2).GetZonePosition() == 0);
	assert(state2.mgr.Get(r4).GetZonePosition() == 1);
	assert(state2.mgr.Get(r3).GetZonePosition() == 2);

	// steal minion
	state2.mgr.GetMinionManipulator(r3).GetZoneChanger().Change(state2, State::kPlayerSecond, Entity::kCardZonePlay, 0);
	assert(state2.mgr.Get(r3).GetZonePosition() == 0);
	assert(state2.mgr.Get(r1).GetZonePosition() == 1);

	assert(state2.mgr.Get(r2).GetZonePosition() == 0);
	assert(state2.mgr.Get(r4).GetZonePosition() == 1);

	auto manipulator = state.mgr.GetMinionManipulator(r1);
	auto ref1 = manipulator.GetEnchantmentHelper().CreateAndAdd<Enchantment::AddAttack>();
	auto ref2 = manipulator.GetEnchantmentHelper().CreateAndAdd<Enchantment::AddAttack_Tier2>();
	auto ref3 = manipulator.GetEnchantmentHelper().CreateAndAdd<Enchantment::AddAttack>();
	auto ref4 = manipulator.GetEnchantmentHelper().CreateAndAdd<Enchantment::AddAttack_Tier2>();

	auto ref5 = manipulator.GetEnchantmentHelper().CreateAndAdd<Enchantment::AddAttack_Aura>();
	auto ref6 = manipulator.GetEnchantmentHelper().CreateAndAdd<Enchantment::AddAttack_Aura>();
	auto ref7 = manipulator.GetEnchantmentHelper().CreateAndAdd<Enchantment::AddAttack_Aura>();

	auto state3 = state;

	state3.mgr.GetMinionManipulator(r1).GetEnchantmentHelper().Remove<Enchantment::AddAttack_Aura>(ref6);
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
	State::State state;

	Entity::RawCard c1;
	c1.card_type = Entity::kCardTypeMinion;
	c1.card_id = "card_id_1";
	c1.enchanted_states.zone = Entity::kCardZoneDeck;
	c1.enchanted_states.cost = 5;
	CardRef r1 = state.mgr.PushBack(state, Entity::Card(c1));

	Entity::RawCard c2;
	c2.card_type = Entity::kCardTypeMinion;
	c2.card_id = "card_id_2";
	c2.enchanted_states.zone = Entity::kCardZoneDeck;
	c2.enchanted_states.cost = 5;
	CardRef r2 = state.mgr.PushBack(state, Entity::Card(c2));

	Entity::RawCard c3;
	c3.card_type = Entity::kCardTypeMinion;
	c3.card_id = "card_id_3";
	c3.enchanted_states.zone = Entity::kCardZoneDeck;
	c3.enchanted_states.cost = 9;
	CardRef r3 = state.mgr.PushBack(state, Entity::Card(c3));

	typedef AuraHelper<Enchantment::AddAttack_Aura> ClientAuraHelper;
	ClientAuraHelper client_aura_helper(r1, r2);
	state.mgr.GetMinionManipulator(r3).GetAuraHelper().Update(client_aura_helper);

	state.mgr.GetMinionManipulator(r3).GetAuraHelper().Update(client_aura_helper);

	auto state2 = state;

	ClientAuraHelper client_aura_helper2(r1, r3);
	state2.mgr.GetMinionManipulator(r3).GetAuraHelper().Update(client_aura_helper2);
}

int main()
{
	test1();
	//test2();

	return 0;
}