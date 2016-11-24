#pragma warning( disable : 4267)  

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

	CardRef GetBattlecryTarget(State::State & state, CardRef card_ref, const State::Cards::Card & card)
	{
		return CardRef();
	}

private:
	int hand_pos_;
};

class RandomGenerator
{
public:
	int Get(int exclusive_max) { return 0; }

	int Get(int min, int max)
	{
		return max;
	}
};

static void CheckZoneAndPosition(const State::State & state, CardRef ref, State::PlayerIdentifier player, State::CardZone zone, int pos)
{
	auto & item = state.mgr.Get(ref);
	assert(item.GetPlayerIdentifier() == player);
	assert(item.GetZone() == zone);
	assert(item.GetZonePosition() == pos);
}

static State::Cards::RawCard GetCard1(State::PlayerIdentifier player, int zone_pos)
{
	State::Cards::RawCard c1;
	c1.card_type = State::kCardTypeMinion;
	c1.card_id = 1;
	c1.zone_position = zone_pos;
	c1.enchanted_states.player = player;
	c1.enchanted_states.zone = State::kCardZoneHand;
	c1.enchanted_states.cost = 5;
	return c1;
}

static State::Cards::RawCard GetCard2(State::PlayerIdentifier player, int zone_pos)
{
	State::Cards::RawCard c1;
	c1.card_type = State::kCardTypeMinion;
	c1.card_id = 2;
	c1.zone_position = zone_pos;
	c1.enchanted_states.player = player;
	c1.enchanted_states.zone = State::kCardZoneHand;
	c1.enchanted_states.cost = 1;
	return c1;
}

static State::Cards::RawCard GetCard3(State::PlayerIdentifier player, int zone_pos)
{
	State::Cards::RawCard c1;
	c1.card_type = State::kCardTypeMinion;
	c1.card_id = 3;
	c1.zone_position = zone_pos;
	c1.enchanted_states.player = player;
	c1.enchanted_states.zone = State::kCardZoneDeck;
	c1.enchanted_states.cost = 2;
	return c1;
}

static void MakeDeck(State::State & state, State::PlayerIdentifier player)
{
	CardRef r1 = state.mgr.PushBack(state, State::Cards::Card(GetCard3(player, state.board.players.Get(player).deck_.Size())));
	CheckZoneAndPosition(state, r1, player, State::kCardZoneDeck, 0);

	CardRef r2 = state.mgr.PushBack(state, State::Cards::Card(GetCard3(player, state.board.players.Get(player).deck_.Size())));
	CheckZoneAndPosition(state, r1, player, State::kCardZoneDeck, 0);
	CheckZoneAndPosition(state, r2, player, State::kCardZoneDeck, 1);

	CardRef r3 = state.mgr.PushBack(state, State::Cards::Card(GetCard3(player, state.board.players.Get(player).deck_.Size())));
	CheckZoneAndPosition(state, r1, player, State::kCardZoneDeck, 0);
	CheckZoneAndPosition(state, r2, player, State::kCardZoneDeck, 1);
	CheckZoneAndPosition(state, r3, player, State::kCardZoneDeck, 2);

	CardRef r4 = state.mgr.PushBack(state, State::Cards::Card(GetCard3(player, state.board.players.Get(player).deck_.Size())));
	CheckZoneAndPosition(state, r1, player, State::kCardZoneDeck, 0);
	CheckZoneAndPosition(state, r2, player, State::kCardZoneDeck, 1);
	CheckZoneAndPosition(state, r3, player, State::kCardZoneDeck, 2);
	CheckZoneAndPosition(state, r4, player, State::kCardZoneDeck, 3);

	CardRef r5 = state.mgr.PushBack(state, State::Cards::Card(GetCard3(player, state.board.players.Get(player).deck_.Size())));
	CheckZoneAndPosition(state, r1, player, State::kCardZoneDeck, 0);
	CheckZoneAndPosition(state, r2, player, State::kCardZoneDeck, 1);
	CheckZoneAndPosition(state, r3, player, State::kCardZoneDeck, 2);
	CheckZoneAndPosition(state, r4, player, State::kCardZoneDeck, 3);
	CheckZoneAndPosition(state, r5, player, State::kCardZoneDeck, 4);
}

static void MakeHand(State::State & state, State::PlayerIdentifier player)
{
	CardRef r1 = state.mgr.PushBack(state, State::Cards::Card(GetCard1(player, state.board.players.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, State::kCardZoneHand, 0);

	CardRef r2 = state.mgr.PushBack(state, State::Cards::Card(GetCard1(player, state.board.players.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, State::kCardZoneHand, 0);
	CheckZoneAndPosition(state, r2, player, State::kCardZoneHand, 1);

	CardRef r3 = state.mgr.PushBack(state, State::Cards::Card(GetCard1(player, state.board.players.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, State::kCardZoneHand, 0);
	CheckZoneAndPosition(state, r2, player, State::kCardZoneHand, 1);
	CheckZoneAndPosition(state, r3, player, State::kCardZoneHand, 2);

	CardRef r4 = state.mgr.PushBack(state, State::Cards::Card(GetCard2(player, state.board.players.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, State::kCardZoneHand, 0);
	CheckZoneAndPosition(state, r2, player, State::kCardZoneHand, 1);
	CheckZoneAndPosition(state, r3, player, State::kCardZoneHand, 2);
	CheckZoneAndPosition(state, r4, player, State::kCardZoneHand, 3);

	CardRef r5 = state.mgr.PushBack(state, State::Cards::Card(GetCard2(player, state.board.players.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, State::kCardZoneHand, 0);
	CheckZoneAndPosition(state, r2, player, State::kCardZoneHand, 1);
	CheckZoneAndPosition(state, r3, player, State::kCardZoneHand, 2);
	CheckZoneAndPosition(state, r4, player, State::kCardZoneHand, 3);
	CheckZoneAndPosition(state, r5, player, State::kCardZoneHand, 4);
}

static State::Cards::RawCard GetHero(State::PlayerIdentifier player)
{
	State::Cards::RawCard raw_card;
	raw_card.card_id = 8;
	raw_card.card_type = State::kCardTypeHero;
	raw_card.enchanted_states.zone = State::kCardZonePlay;
	raw_card.enchanted_states.max_hp = 30;
	raw_card.enchanted_states.player = player;
	return raw_card;
}

int main(void)
{
	State::State state;

	{
		state.mgr.PushBack(state, State::Cards::Card(GetHero(State::kPlayerFirst)));
		MakeDeck(state, State::kPlayerFirst);
		MakeHand(state, State::kPlayerFirst);
	}

	{
		state.mgr.PushBack(state, State::Cards::Card(GetHero(State::kPlayerSecond)));
		state.board.players.Get(State::kPlayerSecond).fatigue_damage_ = 3;
		//MakeDeck(state, State::kPlayerSecond);
		MakeHand(state, State::kPlayerSecond);
	}

	state.current_player = State::kPlayerFirst;
	state.board.players.Get(State::kPlayerFirst).resource_.SetTotal(8);
	state.board.players.Get(State::kPlayerFirst).resource_.Refill();

	ActionParameterGetter action_parameter;
	RandomGenerator random;

	FlowControl::FlowController<ActionParameterGetter, RandomGenerator> controller(state, action_parameter, random);

	CardRef r1 = state.GetCurrentPlayer().hand_.Get(2);
	action_parameter.SetHandCardPosition(2);
	bool triggered = false;
	State::Events::TriggerTypes::OnMinionPlay::FunctorType on_minion_play_1 = 
		[&triggered] (State::Events::HandlersContainerController & controller, const State::Cards::Card & card) {
		triggered = true;
		//std::cout << "OnMinionPlay event: " << card.GetCardId() << std::endl;
	};
	state.event_mgr.PushBack(State::Events::TriggerTypes::OnMinionPlay(on_minion_play_1));
	triggered = false;
	controller.PlayCard();
	assert(triggered);
	CheckZoneAndPosition(state, r1, State::kPlayerFirst, State::kCardZonePlay, 0);

	CardRef r2 = state.GetCurrentPlayer().hand_.Get(2);
	action_parameter.SetHandCardPosition(2);
	triggered = false;
	controller.PlayCard();
	assert(triggered);
	CheckZoneAndPosition(state, r2, State::kPlayerFirst, State::kCardZonePlay, 0);
	CheckZoneAndPosition(state, r1, State::kPlayerFirst, State::kCardZonePlay, 1);

	assert(state.mgr.Get(state.board.players.Get(State::kPlayerSecond).hero_ref_).GetHP() == 30);
	controller.EndTurn();
	assert(state.board.players.Get(State::kPlayerSecond).resource_.GetTotal() == 1);
	assert(state.board.players.Get(State::kPlayerSecond).resource_.GetCurrent() == 1);
	assert(state.mgr.Get(state.board.players.Get(State::kPlayerSecond).hero_ref_).GetHP() == 26);

	return 0;
}