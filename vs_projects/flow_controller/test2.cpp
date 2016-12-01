#include <assert.h>
#include <iostream>

#include "Cards/Database.h"
#include "Cards/id-map.h"
#include "FlowControl/FlowController.h"
#include "State/State.h"
#include "State/Types.h"

class ActionParameterGetter : public FlowControl::IActionParameterGetter
{
public:
	int GetMinionPutLocation(int min, int max)
	{
		return min;
	}

	state::CardRef GetBattlecryTarget(state::State & state, state::CardRef card_ref, const state::Cards::Card & card, std::vector<state::CardRef> const& targets)
	{
		return state::CardRef();
	}
};

class RandomGenerator : public FlowControl::IRandomGenerator
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

state::Cards::Card CreateDeckCard(Cards::CardId id, state::State & state, state::PlayerIdentifier player)
{
	state::Cards::RawCard raw_card = Cards::Database::GetInstance().Get((int)id);
	raw_card.player = player;
	raw_card.zone = state::kCardZoneDeck;
	raw_card.zone_position = (int)state.board.Get(player).deck_.Size();
	return state::Cards::Card(raw_card);
}

state::CardRef PushBackDeckCard(Cards::CardId id, FlowControl::FlowContext & flow_context, state::State & state, state::PlayerIdentifier player)
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
	PushBackDeckCard(Cards::ID_CS1_069, flow_context, state, player);
	PushBackDeckCard(Cards::ID_CS1_069, flow_context, state, player);
}

state::Cards::Card CreateHandCard(Cards::CardId id, state::State & state, state::PlayerIdentifier player)
{
	state::Cards::RawCard raw_card = Cards::Database::GetInstance().Get((int)id);
	raw_card.player = player;
	raw_card.zone = state::kCardZoneHand;
	raw_card.zone_position = (int)state.board.Get(player).hand_.Size();
	return state::Cards::Card(raw_card);
}

state::CardRef AddHandCard(Cards::CardId id, FlowControl::FlowContext & flow_context, state::State & state, state::PlayerIdentifier player)
{
	int hand_count = (int)state.board.Get(player).hand_.Size();

	auto ref = state.mgr.PushBack(state, flow_context, CreateHandCard(id, state, player));

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
	AddHandCard(Cards::ID_AT_116, flow_context, state, player);
	AddHandCard(Cards::ID_AT_116, flow_context, state, player);
	AddHandCard(Cards::ID_FP1_024, flow_context, state, player);
	AddHandCard(Cards::ID_FP1_024, flow_context, state, player);
	AddHandCard(Cards::ID_AT_118, flow_context, state, player);
	AddHandCard(Cards::ID_AT_118, flow_context, state, player);
}

static state::Cards::RawCard GetHero(state::PlayerIdentifier player)
{
	state::Cards::RawCard raw_card;
	raw_card.card_id = 8;
	raw_card.card_type = state::kCardTypeHero;
	raw_card.zone = state::kCardZonePlay;
	raw_card.max_hp = 30;
	raw_card.player = player;
	raw_card.attack = 0;
	return raw_card;
}

void test2()
{
	std::cout << "Reading json file...";
	assert(Cards::Database::GetInstance().LoadJsonFile("../../include/Cards/cards.json"));
	std::cout << " Done." << std::endl;

	state::State state;
	ActionParameterGetter parameter_getter;
	RandomGenerator random;

	FlowControl::FlowController controller(state, parameter_getter, random);

	state.mgr.PushBack(state, controller.flow_context_, state::Cards::Card(GetHero(state::kPlayerFirst)));
	MakeDeck(state, controller.flow_context_, state::kPlayerFirst);
	MakeHand(state, controller.flow_context_, state::kPlayerFirst);

	state.mgr.PushBack(state, controller.flow_context_, state::Cards::Card(GetHero(state::kPlayerSecond)));
	state.board.Get(state::kPlayerSecond).fatigue_damage_ = 3;
	MakeDeck(state, controller.flow_context_, state::kPlayerSecond);
	MakeHand(state, controller.flow_context_, state::kPlayerSecond);

	state.current_player = state::kPlayerFirst;
	state.board.Get(state::kPlayerFirst).resource_.SetTotal(8);
	state.board.Get(state::kPlayerFirst).resource_.Refill();
}