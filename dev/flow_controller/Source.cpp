#include <iostream>
#include <assert.h>
#include "FlowControl/FlowController.h"

class ActionParameterGetter
{
public:
	void SetHandCardPosition(int pos) { hand_pos_ = pos; }
	int GetHandCardPosition() const { return hand_pos_; }

	int GetMinionPutLocation(int min, int max)
	{
		return min;
	}

private:
	int hand_pos_;
};

class RandomGenerator
{
public:
	int Get(int min, int max)
	{
		return max;
	}
};

static void CheckZoneAndPosition(const State::State & state, CardRef ref, State::PlayerIdentifier player, Entity::CardZone zone, int pos)
{
	auto & item = state.mgr.Get(ref);
	assert(item.GetPlayerIdentifier() == player);
	assert(item.GetZone() == zone);
	assert(item.GetZonePosition() == pos);
}

static Entity::RawCard GetCard1(State::PlayerIdentifier player, int zone_pos)
{
	Entity::RawCard c1;
	c1.card_type = Entity::kCardTypeMinion;
	c1.card_id = "card_id_1";
	c1.zone_position = zone_pos;
	c1.enchanted_states.player = player;
	c1.enchanted_states.zone = Entity::kCardZoneHand;
	c1.enchanted_states.cost = 5;
	return c1;
}

static Entity::RawCard GetCard2(State::PlayerIdentifier player, int zone_pos)
{
	Entity::RawCard c1;
	c1.card_type = Entity::kCardTypeMinion;
	c1.card_id = "card_id_2";
	c1.zone_position = zone_pos;
	c1.enchanted_states.player = player;
	c1.enchanted_states.zone = Entity::kCardZoneHand;
	c1.enchanted_states.cost = 1;
	return c1;
}

static void MakeDeck(State::State & state, State::PlayerIdentifier player)
{
	CardRef r1 = state.mgr.PushBack(state, Entity::Card(GetCard1(player, state.players.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, Entity::kCardZoneHand, 0);

	CardRef r2 = state.mgr.PushBack(state, Entity::Card(GetCard1(player, state.players.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, Entity::kCardZoneHand, 0);
	CheckZoneAndPosition(state, r2, player, Entity::kCardZoneHand, 1);

	CardRef r3 = state.mgr.PushBack(state, Entity::Card(GetCard1(player, state.players.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, Entity::kCardZoneHand, 0);
	CheckZoneAndPosition(state, r2, player, Entity::kCardZoneHand, 1);
	CheckZoneAndPosition(state, r3, player, Entity::kCardZoneHand, 2);

	CardRef r4 = state.mgr.PushBack(state, Entity::Card(GetCard2(player, state.players.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, Entity::kCardZoneHand, 0);
	CheckZoneAndPosition(state, r2, player, Entity::kCardZoneHand, 1);
	CheckZoneAndPosition(state, r3, player, Entity::kCardZoneHand, 2);
	CheckZoneAndPosition(state, r4, player, Entity::kCardZoneHand, 3);

	CardRef r5 = state.mgr.PushBack(state, Entity::Card(GetCard2(player, state.players.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, Entity::kCardZoneHand, 0);
	CheckZoneAndPosition(state, r2, player, Entity::kCardZoneHand, 1);
	CheckZoneAndPosition(state, r3, player, Entity::kCardZoneHand, 2);
	CheckZoneAndPosition(state, r4, player, Entity::kCardZoneHand, 3);
	CheckZoneAndPosition(state, r5, player, Entity::kCardZoneHand, 4);
}

int main(void)
{
	State::State state;

	MakeDeck(state, State::kPlayerFirst);
	MakeDeck(state, State::kPlayerSecond);

	state.SetCurrentPlayer(State::kPlayerFirst);
	state.players.Get(State::kPlayerFirst).resource_.SetTotal(8);
	state.players.Get(State::kPlayerFirst).resource_.Refill();

	ActionParameterGetter action_parameter;
	RandomGenerator random;

	FlowControl::FlowController<ActionParameterGetter, RandomGenerator> controller(state, action_parameter, random);

	CardRef r1 = state.GetCurrentPlayer().hand_.Get(2);
	action_parameter.SetHandCardPosition(2);
	controller.PlayCard();
	CheckZoneAndPosition(state, r1, State::kPlayerFirst, Entity::kCardZonePlay, 0);

	CardRef r2 = state.GetCurrentPlayer().hand_.Get(2);
	action_parameter.SetHandCardPosition(2);
	controller.PlayCard();
	CheckZoneAndPosition(state, r2, State::kPlayerFirst, Entity::kCardZonePlay, 0);
	CheckZoneAndPosition(state, r1, State::kPlayerFirst, Entity::kCardZonePlay, 1);

	return 0;
}