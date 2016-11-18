#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>
#include "State/State.h"

#include "Manipulators/Manipulators.h"

static void CheckZoneAndPosition(const State::State & state, CardRef ref, State::PlayerIdentifier player, Entity::CardZone zone, int pos)
{
	auto & item = state.mgr.Get(ref);
	assert(item.GetPlayerIdentifier() == player);
	assert(item.GetZone() == zone);
	assert(item.GetZonePosition() == pos);
}

struct Enchantment1
{
	static constexpr EnchantmentTiers tier = kEnchantmentTier1;

	Enchantments::ApplyFunctor apply_functor;
};
struct Enchantment2
{
	static constexpr EnchantmentTiers tier = kEnchantmentTier2;

	Enchantments::ApplyFunctor apply_functor;
};

static void test1()
{
	State::State state;

	Entity::RawCard c1;
	c1.card_type = Entity::kCardTypeMinion;
	c1.card_id = 1;
	c1.enchanted_states.player = State::kPlayerFirst;
	c1.enchanted_states.zone = Entity::kCardZoneDeck;
	c1.enchanted_states.cost = 5;
	CardRef r1 = state.mgr.PushBack(state, Entity::Card(c1));
	CheckZoneAndPosition(state, r1, State::kPlayerFirst, Entity::kCardZoneDeck, 0);

	c1.enchanted_states.zone = Entity::kCardZoneGraveyard;
	CheckZoneAndPosition(state, r1, State::kPlayerFirst, Entity::kCardZoneDeck, 0);

	state.mgr.GetMinionManipulator(r1).GetZoneChanger().ChangeTo<Entity::kCardZoneHand>(state, State::kPlayerFirst);
	CheckZoneAndPosition(state, r1, State::kPlayerFirst, Entity::kCardZoneHand, 0);

	auto state2 = state;
	CheckZoneAndPosition(state, r1, State::kPlayerFirst, Entity::kCardZoneHand, 0);
	CheckZoneAndPosition(state2, r1, State::kPlayerFirst, Entity::kCardZoneHand, 0);

	state2.mgr.GetMinionManipulator(r1).GetZoneChanger().ChangeTo<Entity::kCardZonePlay>(state2, State::kPlayerFirst, 0);
	CheckZoneAndPosition(state, r1, State::kPlayerFirst, Entity::kCardZoneHand, 0);
	CheckZoneAndPosition(state2, r1, State::kPlayerFirst, Entity::kCardZonePlay, 0);

	state2.mgr.GetMinionManipulator(r1).SetCost(9);
	assert(state.mgr.Get(r1).GetCost() == 5);
	assert(state2.mgr.Get(r1).GetCost() == 9);

	CardRef r2 = state2.mgr.PushBack(state2, Entity::Card(c1));
	CheckZoneAndPosition(state2, r2, State::kPlayerFirst, Entity::kCardZoneGraveyard, 0);
	state2.mgr.GetMinionManipulator(r2).GetZoneChanger().ChangeTo<Entity::kCardZonePlay>(state2, State::kPlayerFirst, 0);
	CheckZoneAndPosition(state2, r2, State::kPlayerFirst, Entity::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r1, State::kPlayerFirst, Entity::kCardZonePlay, 1);

	CardRef r3 = state2.mgr.PushBack(state2, Entity::Card(c1));
	state2.mgr.GetMinionManipulator(r3).GetZoneChanger().ChangeTo<Entity::kCardZonePlay>(state2, State::kPlayerFirst, 2);
	CheckZoneAndPosition(state2, r2, State::kPlayerFirst, Entity::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r1, State::kPlayerFirst, Entity::kCardZonePlay, 1);
	CheckZoneAndPosition(state2, r3, State::kPlayerFirst, Entity::kCardZonePlay, 2);

	CardRef r4 = state2.mgr.PushBack(state2, Entity::Card(c1));
	state2.mgr.GetMinionManipulator(r4).GetZoneChanger().ChangeTo<Entity::kCardZonePlay>(state2, State::kPlayerFirst, 1);
	CheckZoneAndPosition(state2, r2, State::kPlayerFirst, Entity::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r4, State::kPlayerFirst, Entity::kCardZonePlay, 1);
	CheckZoneAndPosition(state2, r1, State::kPlayerFirst, Entity::kCardZonePlay, 2);
	CheckZoneAndPosition(state2, r3, State::kPlayerFirst, Entity::kCardZonePlay, 3);

	// steal minion
	state2.mgr.GetMinionManipulator(r1).GetZoneChanger().ChangeTo<Entity::kCardZonePlay>(state2, State::kPlayerSecond, 0);
	CheckZoneAndPosition(state2, r2, State::kPlayerFirst, Entity::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r4, State::kPlayerFirst, Entity::kCardZonePlay, 1);
	CheckZoneAndPosition(state2, r3, State::kPlayerFirst, Entity::kCardZonePlay, 2);
	CheckZoneAndPosition(state2, r1, State::kPlayerSecond, Entity::kCardZonePlay, 0);

	// steal minion
	state2.mgr.GetMinionManipulator(r3).GetZoneChanger().ChangeTo<Entity::kCardZonePlay>(state2, State::kPlayerSecond, 0);
	CheckZoneAndPosition(state2, r2, State::kPlayerFirst, Entity::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r4, State::kPlayerFirst, Entity::kCardZonePlay, 1);
	CheckZoneAndPosition(state2, r3, State::kPlayerSecond, Entity::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r1, State::kPlayerSecond, Entity::kCardZonePlay, 1);

	// send to graveyard
	state2.mgr.GetMinionManipulator(r1).GetZoneChanger().ChangeTo<Entity::kCardZoneGraveyard>(state2, State::kPlayerFirst);
	CheckZoneAndPosition(state2, r2, State::kPlayerFirst, Entity::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r4, State::kPlayerFirst, Entity::kCardZonePlay, 1);
	CheckZoneAndPosition(state2, r3, State::kPlayerSecond, Entity::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r1, State::kPlayerFirst, Entity::kCardZoneGraveyard, 0);

	// send to another player's graveyard
	state2.mgr.GetMinionManipulator(r2).GetZoneChanger().ChangeTo<Entity::kCardZoneGraveyard>(state2, State::kPlayerFirst);
	CheckZoneAndPosition(state2, r4, State::kPlayerFirst, Entity::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r3, State::kPlayerSecond, Entity::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r1, State::kPlayerFirst, Entity::kCardZoneGraveyard, 0);
	CheckZoneAndPosition(state2, r2, State::kPlayerFirst, Entity::kCardZoneGraveyard, 1);

	Enchantment1 enchant1{ [](Entity::Card & card) {
		card.SetCost(card.GetCost() + 1);
	} };
	Enchantment2 enchant2{ [](Entity::Card & card) {
		card.SetCost(card.GetCost() * 2);
	} };

	auto manipulator = state.mgr.GetMinionManipulator(r1);
	auto ref1 = manipulator.GetEnchantmentHelper().Add(enchant1);
	auto ref2 = manipulator.GetEnchantmentHelper().Add(enchant2);
	auto ref3 = manipulator.GetEnchantmentHelper().Add(enchant1);
	auto ref4 = manipulator.GetEnchantmentHelper().Add(enchant2);

	auto state3 = state;

	state3.mgr.GetMinionManipulator(r1).GetEnchantmentHelper().Remove<Enchantment2>(ref2);
}

class AuraHelper
{
public:
	struct EnchantmentType
	{
		static constexpr EnchantmentTiers tier = kEnchantmentAura;
		Enchantments::ApplyFunctor apply_functor;
	};

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
	EnchantmentType CreateEnchantmentFor(T&& target)
	{
		static_assert(std::is_same<std::decay_t<T>, CardRef>::value, "Wrong type");

		return EnchantmentType{ [](Entity::Card & card) {
			card.SetCost(card.GetCost() - 1);
		} };
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
	c1.card_id = 1;
	c1.enchanted_states.zone = Entity::kCardZoneDeck;
	c1.enchanted_states.cost = 5;
	CardRef r1 = state.mgr.PushBack(state, Entity::Card(c1));

	Entity::RawCard c2;
	c2.card_type = Entity::kCardTypeMinion;
	c2.card_id = 2;
	c2.enchanted_states.zone = Entity::kCardZoneDeck;
	c2.enchanted_states.cost = 5;
	CardRef r2 = state.mgr.PushBack(state, Entity::Card(c2));

	Entity::RawCard c3;
	c3.card_type = Entity::kCardTypeMinion;
	c3.card_id = 3;
	c3.enchanted_states.zone = Entity::kCardZoneDeck;
	c3.enchanted_states.cost = 9;
	CardRef r3 = state.mgr.PushBack(state, Entity::Card(c3));

	typedef AuraHelper ClientAuraHelper;
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