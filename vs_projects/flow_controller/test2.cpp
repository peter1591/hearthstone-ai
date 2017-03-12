#include <assert.h>
#include <iostream>

#include "FlowControl/FlowController.h"
#include "FlowControl/FlowController-impl.h"

class Test2_ActionParameterGetter : public state::IActionParameterGetter
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

class Test2_RandomGenerator : public state::IRandomGenerator
{
public:
	Test2_RandomGenerator() :called(false), next_rand(0) {}

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

static state::CardRef PushBackDeckCard(Cards::CardId id, state::FlowContext & flow_context, state::State & state, state::PlayerIdentifier player)
{
	int deck_count = (int)state.board.Get(player).deck_.Size();

	((Test2_RandomGenerator&)(flow_context.random_)).next_rand = deck_count;
	((Test2_RandomGenerator&)(flow_context.random_)).called = false;

	auto ref = state.mgr.PushBack(state, flow_context, CreateDeckCard(id, state, player));

	if (deck_count > 0) assert(((Test2_RandomGenerator&)(flow_context.random_)).called);
	++deck_count;

	assert(state.board.Get(player).deck_.Size() == deck_count);
	assert(state.mgr.Get(ref).GetCardId() == id);
	assert(state.mgr.Get(ref).GetPlayerIdentifier() == player);
	assert(state.mgr.Get(ref).GetZone() == state::kCardZoneDeck);

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
	raw_card.zone = state::kCardZoneHand;
	raw_card.zone_position = (int)state.board.Get(player).hand_.Size();

	raw_card.enchantment_aux_data.origin_states = raw_card.enchantable_states;

	return state::Cards::Card(raw_card);
}

static state::CardRef AddHandCard(Cards::CardId id, state::CardType type, state::FlowContext & flow_context, state::State & state, state::PlayerIdentifier player)
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

static void MakeHand(state::State & state, state::FlowContext & flow_context, state::PlayerIdentifier player)
{
	AddHandCard(Cards::ID_EX1_089, state::kCardTypeMinion, flow_context, state, player);
	AddHandCard(Cards::ID_NEW1_038, state::kCardTypeMinion, flow_context, state, player);
	AddHandCard(Cards::ID_EX1_089, state::kCardTypeMinion, flow_context, state, player);
	AddHandCard(Cards::ID_EX1_089, state::kCardTypeMinion, flow_context, state, player);
	AddHandCard(Cards::ID_EX1_089, state::kCardTypeMinion, flow_context, state, player);
	AddHandCard(Cards::ID_NEW1_038, state::kCardTypeMinion, flow_context, state, player);
	AddHandCard(Cards::ID_NEW1_038, state::kCardTypeMinion, flow_context, state, player);
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

void test2()
{
	state::State state;
	Test2_ActionParameterGetter parameter_getter;
	Test2_RandomGenerator random;

	FlowControl::FlowController controller(state, parameter_getter, random);

	state.mgr.PushBack(state, controller.flow_context_, state::Cards::Card(GetHero(state::PlayerIdentifier::First())));
	MakeDeck(state, controller.flow_context_, state::PlayerIdentifier::First());
	MakeHand(state, controller.flow_context_, state::PlayerIdentifier::First());

	state.mgr.PushBack(state, controller.flow_context_, state::Cards::Card(GetHero(state::PlayerIdentifier::Second())));
	state.board.Get(state::PlayerIdentifier::Second()).fatigue_damage_ = 3;
	MakeDeck(state, controller.flow_context_, state::PlayerIdentifier::Second());
	MakeHand(state, controller.flow_context_, state::PlayerIdentifier::Second());

	state.current_player = state::PlayerIdentifier::First();
	state.board.Get(state::PlayerIdentifier::First()).resource_.SetTotal(8);
	state.board.Get(state::PlayerIdentifier::First()).resource_.Refill();
	state.board.Get(state::PlayerIdentifier::Second()).resource_.SetTotal(4);

	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 8,8 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 0,4 });
	CheckMinions(state, state::PlayerIdentifier::First(), {});
	CheckMinions(state, state::PlayerIdentifier::Second(), {});
	assert(controller.PlayCard(0) == FlowControl::kResultNotDetermined);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 8 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 0, 5 });
	CheckMinions(state, state::PlayerIdentifier::First(), { {4, 4, 4} });
	CheckMinions(state, state::PlayerIdentifier::Second(), {});

	state.board.Get(state::PlayerIdentifier::First()).resource_.Refill();

	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 8 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 0, 5 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 4, 4 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), {});
	parameter_getter.next_minion_put_location = 1;
	random.next_rand = 0;
	random.called = false;
	assert(controller.PlayCard(0) == FlowControl::kResultNotDetermined);
	assert(!random.called);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 0, 8 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 0, 5 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 4, 4 }, {7,7,7} });
	CheckMinions(state, state::PlayerIdentifier::Second(), {});

	{
		state::State state2 = state;
		FlowControl::FlowController controller2(state2, parameter_getter, random);

		assert(controller2.Attack(
			state.board.Get(state::PlayerIdentifier::First()).minions_.Get(0),
			state.board.Get(state::PlayerIdentifier::Second()).hero_ref_)
			== FlowControl::kResultInvalid);
	}

	assert(controller.EndTurn() == FlowControl::kResultNotDetermined);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 0, 8 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 6 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 4, 4 },{ 8,8,8 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), {});
	assert(state.current_player == state::PlayerIdentifier::Second());

	{
		state::State state2 = state;
		FlowControl::FlowController controller2(state2, parameter_getter, random);

		assert(controller2.Attack(
			state.board.Get(state::PlayerIdentifier::First()).minions_.Get(0),
			state.board.Get(state::PlayerIdentifier::Second()).hero_ref_)
			== FlowControl::kResultInvalid);
	}

	assert(controller.EndTurn() == FlowControl::kResultNotDetermined);
	assert(state.current_player == state::PlayerIdentifier::First());
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 9 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 6 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 4, 4 },{ 9,9,9 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), {});

	assert(controller.Attack(
		state.board.Get(state::PlayerIdentifier::First()).minions_.Get(1),
		state.board.Get(state::PlayerIdentifier::Second()).hero_ref_)
		== FlowControl::kResultNotDetermined);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 21, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 9 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 6 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 4, 4 },{ 9,9,9 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), {});
	assert(state.current_player == state::PlayerIdentifier::First());
	assert(state.GetCurrentPlayer().hand_.Size() == 6);
	assert(state.board.Get(state::PlayerIdentifier::Second()).hand_.Size() == 8);

	AddHandCard(Cards::ID_CS2_189, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::Second());
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 21, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 9 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 6 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 4, 4 },{ 9, 9, 9 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), {});
	assert(state.current_player == state::PlayerIdentifier::First());
	assert(state.board.Get(state::PlayerIdentifier::Second()).hand_.Size() == 9);
	assert(state.board.Get(state::PlayerIdentifier::Second()).graveyard_.GetTotalMinions() == 0);

	AddHandCard(Cards::ID_CS2_189, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::Second());
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 21, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 9 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 6 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 4, 4 },{ 9, 9, 9 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), {});
	assert(state.current_player == state::PlayerIdentifier::First());
	assert(state.board.Get(state::PlayerIdentifier::Second()).hand_.Size() == 10);
	assert(state.board.Get(state::PlayerIdentifier::Second()).graveyard_.GetTotalMinions() == 0);

	assert(controller.EndTurn() == FlowControl::kResultNotDetermined);
	assert(state.current_player == state::PlayerIdentifier::Second());
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 21, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 9 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 7 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 4, 4 },{ 10, 10, 10 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), {});
	assert(state.GetCurrentPlayer().hand_.Size() == 10);
	assert(state.board.Get(state::PlayerIdentifier::Second()).graveyard_.GetTotalMinions() == 1);

	assert(state.mgr.Get(state.GetCurrentPlayer().hand_.Get(9)).GetCardId() == Cards::ID_CS2_189);
	parameter_getter.next_battlecry_target_count = 4;
	parameter_getter.next_battlecry_target_idx = 2;
	assert(controller.PlayCard(9) == FlowControl::kResultNotDetermined);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 21, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 9 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 7 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 4, 4 },{ 10, 9, 10 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { {1, 1, 1} });
	assert(state.GetCurrentPlayer().hand_.Size() == 9);
	assert(state.board.Get(state::PlayerIdentifier::Second()).graveyard_.GetTotalMinions() == 1);

	AddHandCard(Cards::ID_EX1_508, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::Second());
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 21, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 9 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 7 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 4, 4 },{ 10, 9, 10 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 10);
	assert(state.board.Get(state::PlayerIdentifier::Second()).graveyard_.GetTotalMinions() == 1);
	assert(state.mgr.Get(state.GetCurrentPlayer().hand_.Get(9)).GetCardId() == Cards::ID_EX1_508);

	assert(controller.PlayCard(9) == FlowControl::kResultNotDetermined);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 21, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 9 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 5, 7 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 4, 4 },{ 10, 9, 10 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 }, {1, 1, 1} });
	assert(state.GetCurrentPlayer().hand_.Size() == 9);
	assert(state.board.Get(state::PlayerIdentifier::Second()).graveyard_.GetTotalMinions() == 1);

	AddHandCard(Cards::ID_CS2_168, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::Second());
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 21, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 9 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 5, 7 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 4, 4 },{ 10, 9, 10 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 }, {1, 1, 1} });
	assert(state.GetCurrentPlayer().hand_.Size() == 10);
	assert(state.board.Get(state::PlayerIdentifier::Second()).graveyard_.GetTotalMinions() == 1);
	assert(state.mgr.Get(state.GetCurrentPlayer().hand_.Get(9)).GetCardId() == Cards::ID_CS2_168);

	parameter_getter.next_minion_put_location = 1;
	assert(controller.PlayCard(9) == FlowControl::kResultNotDetermined);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 21, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 9 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 4, 7 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 4, 4 },{ 10, 9, 10 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 }, {3, 1, 1}, { 1, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 9);
	assert(state.board.Get(state::PlayerIdentifier::Second()).graveyard_.GetTotalMinions() == 1);

	assert(controller.EndTurn() == FlowControl::kResultNotDetermined);
	assert(controller.EndTurn() == FlowControl::kResultNotDetermined);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 21, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 8, 8 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 4, 4 },{ 12, 11, 12 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 },{ 3, 1, 1 }, {1, 1, 1} });
	assert(state.GetCurrentPlayer().hand_.Size() == 10);
	assert(state.board.Get(state::PlayerIdentifier::Second()).graveyard_.GetTotalMinions() == 1);

	assert(controller.Attack(state.GetCurrentPlayer().minions_.Get(2), state.GetOppositePlayer().minions_.Get(1)) == FlowControl::kResultNotDetermined);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 21, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 8, 8 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 4, 4 },{ 12, 10, 12 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 },{ 2, 1, 1 }});
	assert(state.GetCurrentPlayer().hand_.Size() == 10);
	assert(state.board.Get(state::PlayerIdentifier::Second()).graveyard_.GetTotalMinions() == 2);

	parameter_getter.next_minion_put_location = 1;
	assert(controller.PlayCard(7) == FlowControl::kResultNotDetermined);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 21, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 8 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 4, 4 },{ 12, 10, 12 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 }, {1, 1, 1}, { 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 9);

	assert(controller.Attack(state.GetCurrentPlayer().minions_.Get(1), state.GetOppositePlayer().minions_.Get(0)) == FlowControl::kResultNotDetermined);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 21, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 8 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 3, 4 },{ 12, 10, 12 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 9);

	AddHandCard(Cards::ID_EX1_011, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::Second());
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 21, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 8 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 3, 4 }, { 12, 10, 12 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 }, { 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 10);

	parameter_getter.next_minion_put_location = 0;
	parameter_getter.next_battlecry_target_count = 6;
	parameter_getter.next_battlecry_target_idx = 3;
	assert(controller.PlayCard(9) == FlowControl::kResultNotDetermined);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 23, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 8 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 3, 4 },{ 12, 10, 12 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { {2, 1, 1}, { 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 9);

	AddHandCard(Cards::ID_CS2_106, state::kCardTypeWeapon, controller.flow_context_, state, state::PlayerIdentifier::Second());
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 23, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 8 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 3, 4 },{ 12, 10, 12 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 10);

	assert(controller.PlayCard(9) == FlowControl::kResultNotDetermined);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 23, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 4, 8 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 3, 4 },{ 12, 10, 12 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 9);
	assert(state.mgr.Get(state.GetCurrentPlayer().hero_ref_).GetRawData().weapon_ref.IsValid());

	assert(controller.Attack(
		state.GetCurrentPlayer().hero_ref_, state.GetOppositePlayer().minions_.Get(1))
		== FlowControl::kResultNotDetermined);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 11, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 4, 8 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 3, 4 },{ 12, 7, 12 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 9);
	assert(state.mgr.Get(state.GetCurrentPlayer().hero_ref_).GetRawData().weapon_ref.IsValid());

	assert(controller.EndTurn() == FlowControl::kResultNotDetermined);
	assert(controller.EndTurn() == FlowControl::kResultNotDetermined);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	assert(state.board.Get(state::PlayerIdentifier::Second()).fatigue_damage_ == 4);
	CheckHero(state, state::PlayerIdentifier::Second(), 7, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 9, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 3, 4 }, { 14, 9, 14 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 9);
	assert(state.mgr.Get(state.GetCurrentPlayer().hero_ref_).GetRawData().weapon_ref.IsValid());

	assert(controller.Attack(
		state.GetCurrentPlayer().hero_ref_, state.GetOppositePlayer().minions_.Get(0))
		== FlowControl::kResultNotDetermined);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 3, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 9, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), {{ 14, 9, 14 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 9);
	assert(!state.mgr.Get(state.GetCurrentPlayer().hero_ref_).GetRawData().weapon_ref.IsValid());

	AddHandCard(Cards::ID_CS2_106, state::kCardTypeWeapon, controller.flow_context_, state, state::PlayerIdentifier::Second());
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 3, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 9, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 14, 9, 14 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 10);
	assert(!state.mgr.Get(state.GetCurrentPlayer().hero_ref_).GetRawData().weapon_ref.IsValid());

	assert(controller.PlayCard(9) == FlowControl::kResultNotDetermined);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 3, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 14, 9, 14 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 9);
	assert(state.mgr.Get(state.GetCurrentPlayer().hero_ref_).GetRawData().weapon_ref.IsValid());

	assert(controller.EndTurn() == FlowControl::kResultNotDetermined);
	assert(state.board.Get(state::PlayerIdentifier::First()).fatigue_damage_ == 1);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 3, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 15, 10, 15 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 8);
	assert(state.mgr.Get(state.board.Get(state::PlayerIdentifier::Second()).hero_ref_).GetRawData().weapon_ref.IsValid());

	AddHandCard(Cards::ID_EX1_066, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::First());
	assert(state.board.Get(state::PlayerIdentifier::First()).fatigue_damage_ == 1);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 3, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 15, 10, 15 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 9);
	assert(state.mgr.Get(state.board.Get(state::PlayerIdentifier::Second()).hero_ref_).GetRawData().weapon_ref.IsValid());

	parameter_getter.next_minion_put_location = 1;
	assert(controller.PlayCard(8) == FlowControl::kResultNotDetermined);
	assert(state.board.Get(state::PlayerIdentifier::First()).fatigue_damage_ == 1);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 3, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 15, 10, 15 }, {3, 2, 2} });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 8);
	assert(!state.mgr.Get(state.GetCurrentPlayer().hero_ref_).GetRawData().weapon_ref.IsValid());

	AddHandCard(Cards::ID_CS2_142, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::First());
	assert(state.board.Get(state::PlayerIdentifier::First()).fatigue_damage_ == 1);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 3, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 15, 10, 15 },{ 3, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 9);
	assert(!state.mgr.Get(state.GetCurrentPlayer().hero_ref_).GetRawData().weapon_ref.IsValid());

	parameter_getter.next_minion_put_location = 2;
	assert(controller.PlayCard(8) == FlowControl::kResultNotDetermined);
	assert(state.board.Get(state::PlayerIdentifier::First()).fatigue_damage_ == 1);
	assert(state.board.Get(state::PlayerIdentifier::Second()).fatigue_damage_ == 4);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 3, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 15, 10, 15 },{ 3, 2, 2 }, {2, 2, 2} });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 8);
	assert(!state.mgr.Get(state.GetCurrentPlayer().hero_ref_).GetRawData().weapon_ref.IsValid());

	AddHandCard(Cards::ID_EX1_506, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::First());
	assert(state.board.Get(state::PlayerIdentifier::First()).fatigue_damage_ == 1);
	assert(state.board.Get(state::PlayerIdentifier::Second()).fatigue_damage_ == 4);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 3, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 15, 10, 15 },{ 3, 2, 2 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 9);
	assert(!state.mgr.Get(state.GetCurrentPlayer().hero_ref_).GetRawData().weapon_ref.IsValid());

	parameter_getter.next_minion_put_location = 1;
	assert(controller.PlayCard(8) == FlowControl::kResultNotDetermined);
	assert(state.board.Get(state::PlayerIdentifier::First()).fatigue_damage_ == 1);
	assert(state.board.Get(state::PlayerIdentifier::Second()).fatigue_damage_ == 4);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 3, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 15, 10, 15 }, {2, 1, 1}, {1, 1, 1}, { 3, 2, 2 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 8);
	assert(!state.mgr.Get(state.GetCurrentPlayer().hero_ref_).GetRawData().weapon_ref.IsValid());

	AddHandCard(Cards::ID_EX1_015, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::First());
	assert(state.board.Get(state::PlayerIdentifier::First()).fatigue_damage_ == 1);
	assert(state.board.Get(state::PlayerIdentifier::Second()).fatigue_damage_ == 4);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 3, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 15, 10, 15 },{ 2, 1, 1 },{ 1, 1, 1 },{ 3, 2, 2 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 9);
	assert(!state.mgr.Get(state.GetCurrentPlayer().hero_ref_).GetRawData().weapon_ref.IsValid());

	parameter_getter.next_minion_put_location = 0;
	assert(controller.PlayCard(8) == FlowControl::kResultNotDetermined);
	assert(state.board.Get(state::PlayerIdentifier::First()).fatigue_damage_ == 2);
	assert(state.board.Get(state::PlayerIdentifier::Second()).fatigue_damage_ == 4);
	CheckHero(state, state::PlayerIdentifier::First(), 27, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 3, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { {1, 1, 1}, { 15, 10, 15 },{ 2, 1, 1 },{ 1, 1, 1 },{ 3, 2, 2 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 8);
	assert(!state.mgr.Get(state.GetCurrentPlayer().hero_ref_).GetRawData().weapon_ref.IsValid());

	AddHandCard(Cards::ID_EX1_506, state::kCardTypeMinion, controller.flow_context_, state, state::PlayerIdentifier::First());
	assert(state.board.Get(state::PlayerIdentifier::First()).fatigue_damage_ == 2);
	assert(state.board.Get(state::PlayerIdentifier::Second()).fatigue_damage_ == 4);
	CheckHero(state, state::PlayerIdentifier::First(), 27, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 3, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 1, 1 },{ 15, 10, 15 },{ 2, 1, 1 },{ 1, 1, 1 },{ 3, 2, 2 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 9);
	assert(!state.mgr.Get(state.GetCurrentPlayer().hero_ref_).GetRawData().weapon_ref.IsValid());

	parameter_getter.next_minion_put_location = 4;
	assert(controller.PlayCard(8) == FlowControl::kResultNotDetermined);
	assert(state.board.Get(state::PlayerIdentifier::First()).fatigue_damage_ == 2);
	assert(state.board.Get(state::PlayerIdentifier::Second()).fatigue_damage_ == 4);
	CheckHero(state, state::PlayerIdentifier::First(), 27, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 3, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 0, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 1, 1 },{ 15, 10, 15 },{ 2, 1, 1 },{ 1, 1, 1 },{2, 1, 1}, { 3, 2, 2 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	assert(state.GetCurrentPlayer().hand_.Size() == 8);
	assert(!state.mgr.Get(state.GetCurrentPlayer().hero_ref_).GetRawData().weapon_ref.IsValid());

	assert(controller.EndTurn() == FlowControl::kResultFirstPlayerWin);
}