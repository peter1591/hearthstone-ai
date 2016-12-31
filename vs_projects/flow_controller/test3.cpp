#include <assert.h>
#include <iostream>

#include "Cards/id-map.h"
#include "FlowControl/FlowController.h"
#include "State/State.h"
#include "State/Types.h"

class Test3_ActionParameterGetter : public FlowControl::IActionParameterGetter
{
public:
	int GetMinionPutLocation(int min, int max)
	{
		return next_minion_put_location;
	}

	state::CardRef GetBattlecryTarget(state::State & state, state::CardRef card_ref, const state::Cards::Card & card, std::vector<state::CardRef> const& targets)
	{
		assert(targets.size() == next_battlecry_target_count);

		if (next_battlecry_target_idx < 0) return state::CardRef();
		else return targets[next_battlecry_target_idx];
	}

	int next_minion_put_location;

	int next_battlecry_target_count;
	int next_battlecry_target_idx;
};

class Test3_RandomGenerator : public FlowControl::IRandomGenerator
{
public:
	int Get(int exclusive_max)
	{
		called = true;
		return next_rand;
	}

	int Get(int min, int max)
	{
		called = true;
		return min + next_rand;
	}

public:
	bool called;
	int next_rand;
};

static void CheckZoneAndPosition(const state::State & state, state::CardRef ref, state::PlayerIdentifier player, state::CardZone zone, int pos)
{
	auto & item = state.mgr.Get(ref);
	assert(item.GetPlayerIdentifier() == player);
	assert(item.GetZone() == zone);
	assert(item.GetZonePosition() == pos);
}

static state::Cards::Card CreateDeckCard(Cards::CardId id, state::State & state, state::PlayerIdentifier player)
{
	state::Cards::CardData raw_card = FlowControl::Dispatchers::Minions::CreateInstance(id);
	raw_card.enchantable_states.player = player;
	raw_card.zone = state::kCardZoneDeck;
	raw_card.zone_position = (int)state.board.Get(player).deck_.Size();

	raw_card.enchantment_aux_data.origin_states = raw_card.enchantable_states;

	return state::Cards::Card(raw_card);
}

static state::CardRef PushBackDeckCard(Cards::CardId id, FlowControl::FlowContext & flow_context, state::State & state, state::PlayerIdentifier player)
{
	int deck_count = (int)state.board.Get(player).deck_.Size();

	auto ref = state.mgr.PushBack(state, flow_context, CreateDeckCard(id, state, player));
	++deck_count;

	assert(state.board.Get(player).deck_.Size() == deck_count);
	assert(state.board.Get(player).deck_.Get(deck_count - 1) == ref);
	assert(state.mgr.Get(ref).GetCardId() == id);
	assert(state.mgr.Get(ref).GetPlayerIdentifier() == player);
	assert(state.mgr.Get(ref).GetZone() == state::kCardZoneDeck);
	assert(state.mgr.Get(ref).GetZonePosition() == (deck_count - 1));

	return ref;
}

static void MakeDeck(state::State & state, FlowControl::FlowContext & flow_context, state::PlayerIdentifier player)
{
	PushBackDeckCard(Cards::ID_EX1_020, flow_context, state, player);
	PushBackDeckCard(Cards::ID_EX1_020, flow_context, state, player);
	PushBackDeckCard(Cards::ID_CS2_171, flow_context, state, player);
}

static state::Cards::Card CreateHandCard(Cards::CardId id, state::CardType type, state::State & state, state::PlayerIdentifier player)
{
	state::Cards::CardData raw_card;

	if (type == state::kCardTypeMinion) raw_card = FlowControl::Dispatchers::Minions::CreateInstance(id);
	else if (type == state::kCardTypeWeapon) raw_card = FlowControl::Dispatchers::Weapons::CreateInstance(id);
	else throw std::exception("unknown type");

	raw_card.enchantable_states.player = player;
	raw_card.zone = state::kCardZoneHand;
	raw_card.zone_position = (int)state.board.Get(player).hand_.Size();

	raw_card.enchantment_aux_data.origin_states = raw_card.enchantable_states;

	return state::Cards::Card(raw_card);
}

static state::CardRef AddHandCard(Cards::CardId id, state::CardType type, FlowControl::FlowContext & flow_context, state::State & state, state::PlayerIdentifier player)
{
	int hand_count = (int)state.board.Get(player).hand_.Size();

	auto ref = state.mgr.PushBack(state, flow_context, CreateHandCard(id, type, state, player));

	assert(state.mgr.Get(ref).GetCardId() == id);
	assert(state.mgr.Get(ref).GetPlayerIdentifier() == player);
	if (hand_count == 10) {
		assert(state.board.Get(player).hand_.Size() == 10);
		assert(state.mgr.Get(ref).GetZone() == state::kCardZoneGraveyard);
	}
	else {
		++hand_count;
		assert(state.board.Get(player).hand_.Size() == hand_count);
		assert(state.board.Get(player).hand_.Get(hand_count - 1) == ref);
		assert(state.mgr.Get(ref).GetZone() == state::kCardZoneHand);
		assert(state.mgr.Get(ref).GetZonePosition() == (hand_count - 1));
	}

	return ref;
}

static void MakeHand(state::State & state, FlowControl::FlowContext & flow_context, state::PlayerIdentifier player)
{
	AddHandCard(Cards::ID_CS2_141, state::kCardTypeMinion, flow_context, state, player);
}

static state::Cards::CardData GetHero(state::PlayerIdentifier player)
{
	state::Cards::CardData raw_card;
	raw_card.card_id = 8;
	raw_card.card_type = state::kCardTypeHero;
	raw_card.zone = state::kCardZonePlay;
	raw_card.enchantable_states.max_hp = 30;
	raw_card.enchantable_states.player = player;
	raw_card.enchantable_states.attack = 0;
	raw_card.enchantment_aux_data.origin_states = raw_card.enchantable_states;
	return raw_card;
}

struct MinionCheckStats
{
	int attack;
	int hp;
	int max_hp;
};

static void CheckMinion(state::State &state, state::CardRef ref, MinionCheckStats const& stats)
{
	assert(state.mgr.Get(ref).GetAttack() == stats.attack);
	assert(state.mgr.Get(ref).GetMaxHP() == stats.max_hp);
	assert(state.mgr.Get(ref).GetHP() == stats.hp);
}

static void CheckMinions(state::State & state, state::PlayerIdentifier player, std::vector<MinionCheckStats> const& checking)
{
	std::vector<state::CardRef> const& minions = state.board.Get(player).minions_.Get();

	assert(minions.size() == checking.size());
	for (size_t i = 0; i < minions.size(); ++i) {
		CheckMinion(state, minions[i], checking[i]);
	}
}

struct CrystalCheckStats
{
	int current;
	int total;
};
static void CheckCrystals(state::State & state, state::PlayerIdentifier player, CrystalCheckStats checking)
{
	assert(state.board.Get(player).resource_.GetCurrent() == checking.current);
	assert(state.board.Get(player).resource_.GetTotal() == checking.total);
}

static void CheckHero(state::State & state, state::PlayerIdentifier player, int hp, int armor, int attack)
{
	auto hero_ref = state.board.Get(player).hero_ref_;
	auto const& hero = state.mgr.Get(hero_ref);

	assert(hero.GetHP() == hp);
	// TODO: armor
	assert(hero.GetAttack() == attack);
}

void test3()
{
	state::State state;
	Test3_ActionParameterGetter parameter_getter;
	Test3_RandomGenerator random;

	FlowControl::FlowController controller(state, parameter_getter, random);

	state.mgr.PushBack(state, controller.flow_context_, state::Cards::Card(GetHero(state::kPlayerFirst)));
	MakeDeck(state, controller.flow_context_, state::kPlayerFirst);
	MakeHand(state, controller.flow_context_, state::kPlayerFirst);

	state.mgr.PushBack(state, controller.flow_context_, state::Cards::Card(GetHero(state::kPlayerSecond)));
	MakeDeck(state, controller.flow_context_, state::kPlayerSecond);
	MakeHand(state, controller.flow_context_, state::kPlayerSecond);

	state.current_player = state::kPlayerFirst;
	state.board.Get(state::kPlayerFirst).resource_.SetTotal(4);
	state.board.Get(state::kPlayerFirst).resource_.Refill();
	state.board.Get(state::kPlayerSecond).resource_.SetTotal(4);

	CheckHero(state, state::kPlayerFirst, 30, 0, 0);
	CheckHero(state, state::kPlayerSecond, 30, 0, 0);
	CheckCrystals(state, state::kPlayerFirst, { 4, 4 });
	CheckCrystals(state, state::kPlayerSecond, { 0, 4 });
	CheckMinions(state, state::kPlayerFirst, {});
	CheckMinions(state, state::kPlayerSecond, {});
	assert(state.board.Get(state::kPlayerFirst).hand_.Size() == 1);
	assert(state.board.Get(state::kPlayerSecond).hand_.Size() == 1);

	parameter_getter.next_battlecry_target_count = 2;
	parameter_getter.next_battlecry_target_idx = 0;
	assert(controller.PlayCard(0) == FlowControl::kResultNotDetermined);
	CheckHero(state, state::kPlayerFirst, 29, 0, 0);
	CheckHero(state, state::kPlayerSecond, 30, 0, 0);
	CheckCrystals(state, state::kPlayerFirst, { 1, 4 });
	CheckCrystals(state, state::kPlayerSecond, { 0, 4 });
	CheckMinions(state, state::kPlayerFirst, { {2, 2, 2} });
	CheckMinions(state, state::kPlayerSecond, {});
	assert(state.board.Get(state::kPlayerFirst).hand_.Size() == 0);
	assert(state.board.Get(state::kPlayerSecond).hand_.Size() == 1);

	PushBackDeckCard(Cards::ID_CS2_122, controller.flow_context_, state, state::kPlayerSecond);

	random.next_rand = 3;
	assert(controller.EndTurn() == FlowControl::kResultNotDetermined);
	CheckHero(state, state::kPlayerFirst, 29, 0, 0);
	CheckHero(state, state::kPlayerSecond, 30, 0, 0);
	CheckCrystals(state, state::kPlayerFirst, { 1, 4 });
	CheckCrystals(state, state::kPlayerSecond, { 5, 5 });
	CheckMinions(state, state::kPlayerFirst, { { 2, 2, 2 } });
	CheckMinions(state, state::kPlayerSecond, {});
	assert(state.board.Get(state::kPlayerFirst).hand_.Size() == 0);
	assert(state.board.Get(state::kPlayerSecond).hand_.Size() == 2);

	parameter_getter.next_minion_put_location = 0;
	assert(controller.PlayCard(1) == FlowControl::kResultNotDetermined);
	CheckHero(state, state::kPlayerFirst, 29, 0, 0);
	CheckHero(state, state::kPlayerSecond, 30, 0, 0);
	CheckCrystals(state, state::kPlayerFirst, { 1, 4 });
	CheckCrystals(state, state::kPlayerSecond, { 2, 5 });
	CheckMinions(state, state::kPlayerFirst, { { 2, 2, 2 } });
	CheckMinions(state, state::kPlayerSecond, { { 2, 2, 2} });
	assert(state.board.Get(state::kPlayerFirst).hand_.Size() == 0);
	assert(state.board.Get(state::kPlayerSecond).hand_.Size() == 1);

	PushBackDeckCard(Cards::ID_CS2_122, controller.flow_context_, state, state::kPlayerFirst);

	random.next_rand = 3;
	assert(controller.EndTurn() == FlowControl::kResultNotDetermined);
	CheckHero(state, state::kPlayerFirst, 29, 0, 0);
	CheckHero(state, state::kPlayerSecond, 30, 0, 0);
	CheckCrystals(state, state::kPlayerFirst, { 5, 5 });
	CheckCrystals(state, state::kPlayerSecond, { 2, 5 });
	CheckMinions(state, state::kPlayerFirst, { { 2, 2, 2 } });
	CheckMinions(state, state::kPlayerSecond, { { 2, 2, 2 } });
	assert(state.board.Get(state::kPlayerFirst).hand_.Size() == 1);
	assert(state.board.Get(state::kPlayerSecond).hand_.Size() == 1);

	parameter_getter.next_minion_put_location = 1;
	assert(controller.PlayCard(0) == FlowControl::kResultNotDetermined);
	CheckHero(state, state::kPlayerFirst, 29, 0, 0);
	CheckHero(state, state::kPlayerSecond, 30, 0, 0);
	CheckCrystals(state, state::kPlayerFirst, { 2, 5 });
	CheckCrystals(state, state::kPlayerSecond, { 2, 5 });
	CheckMinions(state, state::kPlayerFirst, { { 3, 2, 2 }, {2, 2, 2} });
	CheckMinions(state, state::kPlayerSecond, { { 2, 2, 2 } });
	assert(state.board.Get(state::kPlayerFirst).hand_.Size() == 0);
	assert(state.board.Get(state::kPlayerSecond).hand_.Size() == 1);

	AddHandCard(Cards::ID_EX1_506, state::kCardTypeMinion, controller.flow_context_, state, state::kPlayerFirst);
	CheckHero(state, state::kPlayerFirst, 29, 0, 0);
	CheckHero(state, state::kPlayerSecond, 30, 0, 0);
	CheckCrystals(state, state::kPlayerFirst, { 2, 5 });
	CheckCrystals(state, state::kPlayerSecond, { 2, 5 });
	CheckMinions(state, state::kPlayerFirst, { { 3, 2, 2 },{ 2, 2, 2 } });
	CheckMinions(state, state::kPlayerSecond, { { 2, 2, 2 } });
	assert(state.board.Get(state::kPlayerFirst).hand_.Size() == 1);
	assert(state.board.Get(state::kPlayerSecond).hand_.Size() == 1);

	parameter_getter.next_minion_put_location = 1;
	assert(controller.PlayCard(0) == FlowControl::kResultNotDetermined);
	CheckHero(state, state::kPlayerFirst, 29, 0, 0);
	CheckHero(state, state::kPlayerSecond, 30, 0, 0);
	CheckCrystals(state, state::kPlayerFirst, { 0, 5 });
	CheckCrystals(state, state::kPlayerSecond, { 2, 5 });
	CheckMinions(state, state::kPlayerFirst, { { 3, 2, 2 }, {3, 1, 1}, {2, 1, 1}, { 2, 2, 2 } });
	CheckMinions(state, state::kPlayerSecond, { { 2, 2, 2 } });
	assert(state.board.Get(state::kPlayerFirst).hand_.Size() == 0);
	assert(state.board.Get(state::kPlayerSecond).hand_.Size() == 1);

	state.board.Get(state::kPlayerFirst).resource_.Refill();
	AddHandCard(Cards::ID_EX1_019, state::kCardTypeMinion, controller.flow_context_, state, state::kPlayerFirst);
	parameter_getter.next_minion_put_location = 0;
	parameter_getter.next_battlecry_target_count = 4;
	parameter_getter.next_battlecry_target_idx = 2;
	assert(controller.PlayCard(0) == FlowControl::kResultNotDetermined);
	CheckHero(state, state::kPlayerFirst, 29, 0, 0);
	CheckHero(state, state::kPlayerSecond, 30, 0, 0);
	CheckCrystals(state, state::kPlayerFirst, { 2, 5 });
	CheckCrystals(state, state::kPlayerSecond, { 2, 5 });
	CheckMinions(state, state::kPlayerFirst, { {4, 2, 2}, { 3, 2, 2 },{ 3, 1, 1 },{ 3, 2, 2 },{ 2, 2, 2 } });
	CheckMinions(state, state::kPlayerSecond, { { 2, 2, 2 } });
	assert(state.board.Get(state::kPlayerFirst).hand_.Size() == 0);
	assert(state.board.Get(state::kPlayerSecond).hand_.Size() == 1);

	{
		auto state2 = state;
		FlowControl::FlowController controller2(state2, parameter_getter, random);
		assert(controller2.Attack(
			state2.board.Get(state::kPlayerFirst).minions_.Get(3),
			state2.board.Get(state::kPlayerSecond).hero_ref_
		) == FlowControl::kResultInvalid);
	}

	random.next_rand = 0;
	assert(controller.EndTurn() == FlowControl::kResultNotDetermined);
	CheckHero(state, state::kPlayerFirst, 29, 0, 0);
	CheckHero(state, state::kPlayerSecond, 30, 0, 0);
	CheckCrystals(state, state::kPlayerFirst, { 2, 5 });
	CheckCrystals(state, state::kPlayerSecond, { 6, 6 });
	CheckMinions(state, state::kPlayerFirst, { { 4, 2, 2 },{ 3, 2, 2 },{ 3, 1, 1 },{ 3, 2, 2 },{ 2, 2, 2 } });
	CheckMinions(state, state::kPlayerSecond, { { 2, 2, 2 } });
	assert(state.board.Get(state::kPlayerFirst).hand_.Size() == 0);
	assert(state.board.Get(state::kPlayerSecond).hand_.Size() == 2);

	AddHandCard(Cards::ID_CS2_124, state::kCardTypeMinion, controller.flow_context_, state, state::kPlayerSecond);
	parameter_getter.next_minion_put_location = 1;
	assert(controller.PlayCard(2) == FlowControl::kResultNotDetermined);
	CheckHero(state, state::kPlayerFirst, 29, 0, 0);
	CheckHero(state, state::kPlayerSecond, 30, 0, 0);
	CheckCrystals(state, state::kPlayerFirst, { 2, 5 });
	CheckCrystals(state, state::kPlayerSecond, { 3, 6 });
	CheckMinions(state, state::kPlayerFirst, { { 4, 2, 2 },{ 3, 2, 2 },{ 3, 1, 1 },{ 3, 2, 2 },{ 2, 2, 2 } });
	CheckMinions(state, state::kPlayerSecond, { { 2, 2, 2 }, {4, 1, 1} });
	assert(state.board.Get(state::kPlayerFirst).hand_.Size() == 0);
	assert(state.board.Get(state::kPlayerSecond).hand_.Size() == 2);

	assert(controller.Attack(
		state.board.Get(state::kPlayerSecond).minions_.Get(1),
		state.board.Get(state::kPlayerFirst).minions_.Get(4)
	) == FlowControl::kResultNotDetermined);
	CheckHero(state, state::kPlayerFirst, 29, 0, 0);
	CheckHero(state, state::kPlayerSecond, 30, 0, 0);
	CheckCrystals(state, state::kPlayerFirst, { 2, 5 });
	CheckCrystals(state, state::kPlayerSecond, { 3, 6 });
	CheckMinions(state, state::kPlayerFirst, { { 3, 2, 2 },{ 2, 2, 2 },{ 2, 1, 1 },{ 2, 2, 2 }});
	CheckMinions(state, state::kPlayerSecond, { { 2, 2, 2 } });
	assert(state.board.Get(state::kPlayerFirst).hand_.Size() == 0);
	assert(state.board.Get(state::kPlayerSecond).hand_.Size() == 2);

	state.board.Get(state::kPlayerSecond).resource_.Refill();
	AddHandCard(Cards::ID_DS1_055, state::kCardTypeMinion, controller.flow_context_, state, state::kPlayerSecond);
	parameter_getter.next_minion_put_location = 1;
	assert(controller.PlayCard(2) == FlowControl::kResultNotDetermined);
	CheckHero(state, state::kPlayerFirst, 29, 0, 0);
	CheckHero(state, state::kPlayerSecond, 30, 0, 0);
	CheckCrystals(state, state::kPlayerFirst, { 2, 5 });
	CheckCrystals(state, state::kPlayerSecond, { 1, 6 });
	CheckMinions(state, state::kPlayerFirst, { { 3, 2, 2 },{ 2, 2, 2 },{ 2, 1, 1 },{ 2, 2, 2 } });
	CheckMinions(state, state::kPlayerSecond, { { 2, 2, 2 }, {5, 5, 5} });
	assert(state.board.Get(state::kPlayerFirst).hand_.Size() == 0);
	assert(state.board.Get(state::kPlayerSecond).hand_.Size() == 2);
}