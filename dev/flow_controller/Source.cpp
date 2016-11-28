#pragma warning( disable : 4267)  

#include <iostream>
#include <assert.h>
#include "FlowControl/FlowController.h"

using state::CardRef;

class ActionParameterGetter
{
public:
	int GetMinionPutLocation(int min, int max)
	{
		return min;
	}

	state::CardRef GetBattlecryTarget(state::State & state, state::CardRef card_ref, const state::Cards::Card & card)
	{
		return state::CardRef();
	}
};

class RandomGenerator
{
public:
	int Get(int exclusive_max)
	{
		return exclusive_max - 1;
	}

	int Get(int min, int max)
	{
		return max;
	}
};

class Card1
{
public:
	static void BattleCry(FlowControl::Context::BattleCry & context)
	{
		debug1 = true;

		context.GetBattleCryTarget();
	}

	static bool debug1;
};
bool Card1::debug1 = true;
REGISTER_MINION_CARD_CLASS(1, Card1)

static void CheckZoneAndPosition(const state::State & state, state::CardRef ref, state::PlayerIdentifier player, state::CardZone zone, int pos)
{
	auto & item = state.mgr.Get(ref);
	assert(item.GetPlayerIdentifier() == player);
	assert(item.GetZone() == zone);
	assert(item.GetZonePosition() == pos);
}

static state::Cards::RawCard GetCard1(state::PlayerIdentifier player, int zone_pos)
{
	state::Cards::RawCard c1;
	c1.card_type = state::kCardTypeMinion;
	c1.card_id = 1;
	c1.zone_position = zone_pos;
	c1.enchanted_states.player = player;
	c1.enchanted_states.zone = state::kCardZoneHand;
	c1.enchanted_states.cost = 5;
	c1.enchanted_states.attack = 7;
	return c1;
}

static state::Cards::RawCard GetCard2(state::PlayerIdentifier player, int zone_pos)
{
	state::Cards::RawCard c1;
	c1.card_type = state::kCardTypeMinion;
	c1.card_id = 2;
	c1.zone_position = zone_pos;
	c1.enchanted_states.player = player;
	c1.enchanted_states.zone = state::kCardZoneHand;
	c1.enchanted_states.cost = 1;
	c1.enchanted_states.attack = 3;
	return c1;
}

static state::Cards::RawCard GetCard3(state::PlayerIdentifier player, int zone_pos)
{
	state::Cards::RawCard c1;
	c1.card_type = state::kCardTypeMinion;
	c1.card_id = 3;
	c1.zone_position = zone_pos;
	c1.enchanted_states.player = player;
	c1.enchanted_states.zone = state::kCardZoneDeck;
	c1.enchanted_states.cost = 2;
	return c1;
}

static void MakeDeck(state::State & state, state::PlayerIdentifier player)
{
	CardRef r1 = state.mgr.PushBack(state, state::Cards::Card(GetCard3(player, state.board.Get(player).deck_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneDeck, 0);

	CardRef r2 = state.mgr.PushBack(state, state::Cards::Card(GetCard3(player, state.board.Get(player).deck_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneDeck, 0);
	CheckZoneAndPosition(state, r2, player, state::kCardZoneDeck, 1);

	CardRef r3 = state.mgr.PushBack(state, state::Cards::Card(GetCard3(player, state.board.Get(player).deck_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneDeck, 0);
	CheckZoneAndPosition(state, r2, player, state::kCardZoneDeck, 1);
	CheckZoneAndPosition(state, r3, player, state::kCardZoneDeck, 2);

	CardRef r4 = state.mgr.PushBack(state, state::Cards::Card(GetCard3(player, state.board.Get(player).deck_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneDeck, 0);
	CheckZoneAndPosition(state, r2, player, state::kCardZoneDeck, 1);
	CheckZoneAndPosition(state, r3, player, state::kCardZoneDeck, 2);
	CheckZoneAndPosition(state, r4, player, state::kCardZoneDeck, 3);

	CardRef r5 = state.mgr.PushBack(state, state::Cards::Card(GetCard3(player, state.board.Get(player).deck_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneDeck, 0);
	CheckZoneAndPosition(state, r2, player, state::kCardZoneDeck, 1);
	CheckZoneAndPosition(state, r3, player, state::kCardZoneDeck, 2);
	CheckZoneAndPosition(state, r4, player, state::kCardZoneDeck, 3);
	CheckZoneAndPosition(state, r5, player, state::kCardZoneDeck, 4);
}

static void MakeHand(state::State & state, state::PlayerIdentifier player)
{
	CardRef r1 = state.mgr.PushBack(state, state::Cards::Card(GetCard1(player, state.board.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneHand, 0);

	CardRef r2 = state.mgr.PushBack(state, state::Cards::Card(GetCard1(player, state.board.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneHand, 0);
	CheckZoneAndPosition(state, r2, player, state::kCardZoneHand, 1);

	CardRef r3 = state.mgr.PushBack(state, state::Cards::Card(GetCard1(player, state.board.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneHand, 0);
	CheckZoneAndPosition(state, r2, player, state::kCardZoneHand, 1);
	CheckZoneAndPosition(state, r3, player, state::kCardZoneHand, 2);

	CardRef r4 = state.mgr.PushBack(state, state::Cards::Card(GetCard2(player, state.board.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneHand, 0);
	CheckZoneAndPosition(state, r2, player, state::kCardZoneHand, 1);
	CheckZoneAndPosition(state, r3, player, state::kCardZoneHand, 2);
	CheckZoneAndPosition(state, r4, player, state::kCardZoneHand, 3);

	CardRef r5 = state.mgr.PushBack(state, state::Cards::Card(GetCard2(player, state.board.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneHand, 0);
	CheckZoneAndPosition(state, r2, player, state::kCardZoneHand, 1);
	CheckZoneAndPosition(state, r3, player, state::kCardZoneHand, 2);
	CheckZoneAndPosition(state, r4, player, state::kCardZoneHand, 3);
	CheckZoneAndPosition(state, r5, player, state::kCardZoneHand, 4);
}

static state::Cards::RawCard GetHero(state::PlayerIdentifier player)
{
	state::Cards::RawCard raw_card;
	raw_card.card_id = 8;
	raw_card.card_type = state::kCardTypeHero;
	raw_card.enchanted_states.zone = state::kCardZonePlay;
	raw_card.enchanted_states.max_hp = 30;
	raw_card.enchanted_states.player = player;
	raw_card.enchanted_states.attack = 0;
	return raw_card;
}

int main(void)
{
	state::State state;

	{
		state.mgr.PushBack(state, state::Cards::Card(GetHero(state::kPlayerFirst)));
		MakeDeck(state, state::kPlayerFirst);
		MakeHand(state, state::kPlayerFirst);
	}

	{
		state.mgr.PushBack(state, state::Cards::Card(GetHero(state::kPlayerSecond)));
		state.board.Get(state::kPlayerSecond).fatigue_damage_ = 3;
		//MakeDeck(state, state::kPlayerSecond);
		MakeHand(state, state::kPlayerSecond);
	}

	state.current_player = state::kPlayerFirst;
	state.board.Get(state::kPlayerFirst).resource_.SetTotal(8);
	state.board.Get(state::kPlayerFirst).resource_.Refill();

	ActionParameterGetter action_parameter;
	RandomGenerator random;

	FlowControl::FlowController<ActionParameterGetter, RandomGenerator> controller(state, action_parameter, random);

	CardRef r1 = state.GetCurrentPlayer().hand_.Get(2);
	bool triggered = false;
	state::Events::EventTypes::OnMinionPlay::FunctorType on_minion_play_1 = 
		[&triggered] (state::Events::HandlersContainerController & controller, const state::Cards::Card & card) {
		triggered = true;
		//std::cout << "OnMinionPlay event: " << card.GetCardId() << std::endl;
	};
	state.event_mgr.PushBack(state::Events::EventTypes::OnMinionPlay(on_minion_play_1));

	triggered = false;
	Card1::debug1 = false;
	controller.PlayCard(2);
	assert(triggered);
	assert(Card1::debug1);
	CheckZoneAndPosition(state, r1, state::kPlayerFirst, state::kCardZonePlay, 0);

	CardRef r2 = state.GetCurrentPlayer().hand_.Get(2);
	triggered = false;
	controller.PlayCard(2);
	assert(triggered);
	CheckZoneAndPosition(state, r2, state::kPlayerFirst, state::kCardZonePlay, 0);
	CheckZoneAndPosition(state, r1, state::kPlayerFirst, state::kCardZonePlay, 1);

	assert(state.mgr.Get(state.board.Get(state::kPlayerSecond).hero_ref_).GetHP() == 30);
	controller.EndTurn();
	assert(state.board.Get(state::kPlayerSecond).resource_.GetTotal() == 1);
	assert(state.board.Get(state::kPlayerSecond).resource_.GetCurrent() == 1);
	assert(state.mgr.Get(state.board.Get(state::kPlayerSecond).hero_ref_).GetHP() == 26);

	{
		CardRef attacker = state.board.Get(state::kPlayerFirst).minions_.Get(0);
		CardRef defender = state.board.Get(state::kPlayerSecond).hero_ref_;
		
		bool debug1 = false;
		state.event_mgr.PushBack(state::Events::EventTypes::BeforeAttack(
			[&state, &attacker, &defender, &debug1](state::Events::HandlersContainerController & controller, state::State & in_state, CardRef & in_attacker, CardRef & in_defender) -> void {
			assert(&state == &in_state);
			assert(attacker == in_attacker);
			assert(defender == in_defender);
			debug1 = true;
		}));

		assert(!debug1);
		assert(state.mgr.Get(attacker).GetAttack() == 3);
		assert(state.mgr.Get(defender).GetDamage() == 4);
		assert(state.mgr.Get(attacker).GetDamage() == 0);
		controller.Attack(attacker, defender);
		assert(debug1);
		assert(state.mgr.Get(attacker).GetDamage() == 0);
		assert(state.mgr.Get(defender).GetDamage() == 7);

		bool debug2 = false;
		state::Events::EventTypes::OnTakeDamage::FunctorType callback1 =
			[&debug2, defender] (state::Events::HandlersContainerController & controller, FlowControl::Context::OnTakeDamage & context) {
			if (context.card_ref_ == defender) {
				debug2 = true;
				context.damage_ = 1;
			}
		};

		state.event_mgr.PushBack(state::Events::EventTypes::OnTakeDamage(callback1));

		debug1 = false;
		controller.Attack(attacker, defender);
		assert(debug1);
		assert(debug2);
		assert(state.mgr.Get(attacker).GetDamage() == 0);
		assert(state.mgr.Get(defender).GetDamage() == 8);
	}

	return 0;
}