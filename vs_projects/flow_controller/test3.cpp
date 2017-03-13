#include <assert.h>
#include <iostream>

#include "FlowControl/FlowController.h"
#include "FlowControl/FlowController-impl.h"

class Test3_ActionParameterGetter : public state::IActionParameterGetter
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

class Test3_RandomGenerator : public state::IRandomGenerator
{
public:
	Test3_RandomGenerator() :called(false), next_rand(0) {}

	int Get(int exclusive_max)
	{
		called = true;
		return next_rand;
	}

	size_t Get(size_t exclusive_max) { return (size_t)Get((int)exclusive_max); }

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
	auto & item = state.GetCardsManager().Get(ref);
	assert(item.GetPlayerIdentifier() == player);
	assert(item.GetZone() == zone);
	assert(item.GetZonePosition() == pos);
}

static state::Cards::Card CreateDeckCard(Cards::CardId id, state::State & state, state::PlayerIdentifier player)
{
	state::Cards::CardData raw_card = FlowControl::Dispatchers::Minions::CreateInstance(id);
	raw_card.enchantable_states.player = player;
	raw_card.zone = state::kCardZoneInvalid;
	raw_card.enchantment_aux_data.origin_states = raw_card.enchantable_states;

	return state::Cards::Card(raw_card);
}

static state::CardRef PushBackDeckCard(Cards::CardId id, state::FlowContext & flow_context, state::State & state, state::PlayerIdentifier player)
{
	int deck_count = (int)state.GetBoard().Get(player).deck_.Size();

	((Test3_RandomGenerator&)(flow_context.random_)).next_rand = deck_count;
	((Test3_RandomGenerator&)(flow_context.random_)).called = false;

	auto ref = state.GetCardsManager().PushBack(CreateDeckCard(id, state, player));
	FlowControl::Manipulate(state, flow_context).Card(ref).Zone().WithZone<state::kCardZoneInvalid>()
		.ChangeTo<state::kCardZoneDeck>(player);

	if (deck_count > 0) assert(((Test3_RandomGenerator&)(flow_context.random_)).called);
	++deck_count;

	assert(state.GetBoard().Get(player).deck_.Size() == deck_count);
	assert(state.GetCardsManager().Get(ref).GetCardId() == id);
	assert(state.GetCardsManager().Get(ref).GetPlayerIdentifier() == player);
	assert(state.GetCardsManager().Get(ref).GetZone() == state::kCardZoneDeck);

	return ref;
}

static void MakeDeck(state::State & state, state::FlowContext & flow_context, state::PlayerIdentifier player)
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
	raw_card.zone = state::kCardZoneInvalid;
	raw_card.enchantment_aux_data.origin_states = raw_card.enchantable_states;

	return state::Cards::Card(raw_card);
}

static state::CardRef AddHandCard(Cards::CardId id, state::CardType type, state::FlowContext & flow_context, state::State & state, state::PlayerIdentifier player)
{
	int hand_count = (int)state.GetBoard().Get(player).hand_.Size();

	auto ref = state.GetCardsManager().PushBack(CreateHandCard(id, type, state, player));
	FlowControl::Manipulate(state, flow_context).Card(ref).Zone().WithZone<state::kCardZoneInvalid>()
		.ChangeTo<state::kCardZoneHand>(player);

	assert(state.GetCardsManager().Get(ref).GetCardId() == id);
	assert(state.GetCardsManager().Get(ref).GetPlayerIdentifier() == player);
	if (hand_count == 10) {
		assert(state.GetBoard().Get(player).hand_.Size() == 10);
		assert(state.GetCardsManager().Get(ref).GetZone() == state::kCardZoneGraveyard);
	}
	else {
		++hand_count;
		assert(state.GetBoard().Get(player).hand_.Size() == hand_count);
		assert(state.GetBoard().Get(player).hand_.Get(hand_count - 1) == ref);
		assert(state.GetCardsManager().Get(ref).GetZone() == state::kCardZoneHand);
		assert(state.GetCardsManager().Get(ref).GetZonePosition() == (hand_count - 1));
	}

	return ref;
}

static void MakeHand(state::State & state, state::FlowContext & flow_context, state::PlayerIdentifier player)
{
	AddHandCard(Cards::ID_CS2_141, state::kCardTypeMinion, flow_context, state, player);
}

static void MakeHero(state::State & state, state::FlowContext & flow_context, state::PlayerIdentifier player)
{
	state::Cards::CardData raw_card;
	raw_card.card_id = 8;
	raw_card.card_type = state::kCardTypeHero;
	raw_card.zone = state::kCardZoneInvalid;
	raw_card.enchantable_states.max_hp = 30;
	raw_card.enchantable_states.player = player;
	raw_card.enchantable_states.attack = 0;
	raw_card.enchantment_aux_data.origin_states = raw_card.enchantable_states;

	state::CardRef ref = state.GetCardsManager().PushBack(state::Cards::Card(raw_card));

	FlowControl::Manipulate(state, flow_context).Hero(ref).Zone().WithZone<state::kCardZoneInvalid>()
		.ChangeTo<state::kCardZonePlay>(player);
}

struct MinionCheckStats
{
	int attack;
	int hp;
	int max_hp;
};

static void CheckMinion(state::State &state, state::CardRef ref, MinionCheckStats const& stats)
{
	assert(state.GetCardsManager().Get(ref).GetAttack() == stats.attack);
	assert(state.GetCardsManager().Get(ref).GetMaxHP() == stats.max_hp);
	assert(state.GetCardsManager().Get(ref).GetHP() == stats.hp);
}

static void CheckMinions(state::State & state, state::PlayerIdentifier player, std::vector<MinionCheckStats> const& checking)
{
	std::vector<state::CardRef> const& minions = state.GetBoard().Get(player).minions_.Get();

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
	assert(state.GetBoard().Get(player).resource_.GetCurrent() == checking.current);
	assert(state.GetBoard().Get(player).resource_.GetTotal() == checking.total);
}

static void CheckHero(state::State & state, state::PlayerIdentifier player, int hp, int armor, int attack)
{
	auto hero_ref = state.GetBoard().Get(player).hero_ref_;
	auto const& hero = state.GetCardsManager().Get(hero_ref);

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

	MakeHero(state, controller.flow_context_, state::PlayerIdentifier::First());
	MakeDeck(state, controller.flow_context_, state::PlayerIdentifier::First());
	MakeHand(state, controller.flow_context_, state::PlayerIdentifier::First());

	MakeHero(state, controller.flow_context_, state::PlayerIdentifier::Second());
	MakeDeck(state, controller.flow_context_, state::PlayerIdentifier::Second());
	MakeHand(state, controller.flow_context_, state::PlayerIdentifier::Second());

	state.GetMutableCurrentPlayerId().SetFirst();
	state.GetBoard().GetFirst().resource_.SetTotal(4);
	state.GetBoard().GetFirst().resource_.Refill();
	state.GetBoard().GetSecond().resource_.SetTotal(4);

	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 4 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 0, 4 });
	CheckMinions(state, state::PlayerIdentifier::First(), {});
	CheckMinions(state, state::PlayerIdentifier::Second(), {});
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

	parameter_getter.next_battlecry_target_count = 2;
	parameter_getter.next_battlecry_target_idx = 0;
	if (controller.PlayCard(0) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 4 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 0, 4 });
	CheckMinions(state, state::PlayerIdentifier::First(), { {2, 2, 2} });
	CheckMinions(state, state::PlayerIdentifier::Second(), {});
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

	PushBackDeckCard(Cards::ID_CS2_122, controller.flow_context_, state, state::PlayerIdentifier::Second());

	random.next_rand = 3;
	if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 4 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 5, 5 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), {});
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 2);

	parameter_getter.next_minion_put_location = 0;
	if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 4 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 2, 5 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 2, 2} });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

	PushBackDeckCard(Cards::ID_CS2_122, controller.flow_context_, state, state::PlayerIdentifier::First());

	random.next_rand = 3;
	if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 5 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 2, 5 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 2, 2 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

	parameter_getter.next_minion_put_location = 1;
	if (controller.PlayCard(0) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 5 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 2, 5 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 3, 2, 2 }, {2, 2, 2} });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 2, 2 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

	AddHandCard(Cards::ID_EX1_506, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::First());
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 5 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 2, 5 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 3, 2, 2 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 2, 2 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

	parameter_getter.next_minion_put_location = 1;
	if (controller.PlayCard(0) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 0, 5 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 2, 5 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 3, 2, 2 }, {3, 1, 1}, {2, 1, 1}, { 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 2, 2 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

	state.GetBoard().Get(state::PlayerIdentifier::First()).resource_.Refill();
	AddHandCard(Cards::ID_EX1_019, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::First());
	parameter_getter.next_minion_put_location = 0;
	parameter_getter.next_battlecry_target_count = 4;
	parameter_getter.next_battlecry_target_idx = 2;
	if (controller.PlayCard(0) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 5 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 2, 5 });
	CheckMinions(state, state::PlayerIdentifier::First(), { {4, 2, 2}, { 3, 2, 2 },{ 3, 1, 1 },{ 3, 2, 2 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 2, 2 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

	{
		auto state2 = state;
		FlowControl::FlowController controller2(state2, parameter_getter, random);
		if (controller2.Attack(
			state2.GetBoard().Get(state::PlayerIdentifier::First()).minions_.Get(3),
			state2.GetBoard().Get(state::PlayerIdentifier::Second()).hero_ref_
		) != FlowControl::kResultInvalid) assert(false);
	}

	random.next_rand = 0;
	if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 5 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 6 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 2, 2 },{ 3, 2, 2 },{ 3, 1, 1 },{ 3, 2, 2 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 2, 2 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 2);

	AddHandCard(Cards::ID_CS2_124, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::Second());
	parameter_getter.next_minion_put_location = 1;
	if (controller.PlayCard(2) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 5 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 3, 6 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 2, 2 },{ 3, 2, 2 },{ 3, 1, 1 },{ 3, 2, 2 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 2, 2 }, {4, 1, 1} });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 2);

	if (controller.Attack(
		state.GetBoard().Get(state::PlayerIdentifier::Second()).minions_.Get(1),
		state.GetBoard().Get(state::PlayerIdentifier::First()).minions_.Get(4)
	) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 5 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 3, 6 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 3, 2, 2 },{ 2, 2, 2 },{ 2, 1, 1 },{ 2, 2, 2 }});
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 2, 2 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 2);

	state.GetBoard().Get(state::PlayerIdentifier::Second()).resource_.Refill();
	AddHandCard(Cards::ID_DS1_055, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::Second());
	parameter_getter.next_minion_put_location = 1;
	if (controller.PlayCard(2) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 5 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 1, 6 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 3, 2, 2 },{ 2, 2, 2 },{ 2, 1, 1 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 2, 2 }, {5, 5, 5} });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 2);

	state.GetBoard().Get(state::PlayerIdentifier::Second()).resource_.Refill();
	AddHandCard(Cards::ID_CS2_226, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::Second());
	parameter_getter.next_minion_put_location = 1;
	if (controller.PlayCard(2) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 5 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 1, 6 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 3, 2, 2 },{ 2, 2, 2 },{ 2, 1, 1 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 2, 2 }, {7, 6, 6}, { 5, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 2);

	if (controller.Attack(
		state.GetBoard().Get(state::PlayerIdentifier::Second()).minions_.Get(0),
		state.GetBoard().Get(state::PlayerIdentifier::First()).minions_.Get(1))
		!= FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 5 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 1, 6 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 3, 2, 2 },{ 2, 1, 1 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 6, 6, 6 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 2);

	state.GetBoard().Get(state::PlayerIdentifier::Second()).resource_.Refill();
	AddHandCard(Cards::ID_EX1_399, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::Second());
	parameter_getter.next_minion_put_location = 1;
	if (controller.PlayCard(2) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 5 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 1, 6 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 3, 2, 2 },{ 2, 1, 1 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 6, 6, 6 },{2, 7, 7}, { 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 2);

	state.GetBoard().Get(state::PlayerIdentifier::Second()).resource_.Refill();
	AddHandCard(Cards::ID_CS2_189, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::Second());
	parameter_getter.next_battlecry_target_count = 8;
	parameter_getter.next_battlecry_target_idx = 6;
	parameter_getter.next_minion_put_location = 0;
	if (controller.PlayCard(2) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 5 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 5, 6 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 3, 2, 2 },{ 2, 1, 1 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { {1, 1, 1},  { 6, 6, 6 },{ 5, 6, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 2);

	AddHandCard(Cards::ID_CS2_189, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::Second());
	parameter_getter.next_battlecry_target_count = 9;
	parameter_getter.next_battlecry_target_idx = 7;
	parameter_getter.next_minion_put_location = 0;
	if (controller.PlayCard(2) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 5 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 4, 6 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 3, 2, 2 },{ 2, 1, 1 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 }, {1, 1, 1},{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 2);

	state.GetBoard().Get(state::PlayerIdentifier::Second()).resource_.Refill();
	AddHandCard(Cards::ID_EX1_593, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::Second());
	parameter_getter.next_minion_put_location = 1;
	if (controller.PlayCard(2) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 5 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 1, 6 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 3, 2, 2 },{ 2, 1, 1 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 }, {4, 4, 4}, { 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 2);

	if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 6 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 1, 6 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 3, 2, 2 },{ 2, 1, 1 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 },{ 4, 4, 4 },{ 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 2);

	if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 6 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 7 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 3, 2, 2 },{ 2, 1, 1 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 },{ 4, 4, 4 },{ 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 3);

	AddHandCard(Cards::ID_CS2_222, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::Second());
	parameter_getter.next_minion_put_location = 1;
	if (controller.PlayCard(3) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 6 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 0, 7 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 3, 2, 2 },{ 2, 1, 1 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 2, 2 }, {6, 6, 6}, { 5, 5, 5 },{ 2, 2, 2 },{ 7, 7, 7 },{ 9, 6, 8 },{ 5, 6, 6 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 3);
}