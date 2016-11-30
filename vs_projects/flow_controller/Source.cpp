#pragma warning( disable : 4267)  

#include <iostream>
#include <assert.h>
#include "FlowControl/FlowController.h"
#include "FlowControl/Dispatchers/MinionCardBase.h"

using state::CardRef;

class ActionParameterGetter : public FlowControl::IActionParameterGetter
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

class RandomGenerator : public FlowControl::IRandomGenerator
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

class Card1 : MinionCardBase
{
public:
	static void BattleCry(FlowControl::Context::BattleCry & context)
	{
		debug1 = true;

		context.GetBattleCryTarget();
	}

	static void AfterSummoned(FlowControl::Context::AfterSummoned & context)
	{
		debug2 = true;

		Deathrattles(context).Add([](auto& context) {
			Damage(context).Opponent().Amount(2);
		});
	}

	static bool debug1;
	static bool debug2;
};
bool Card1::debug1 = false;
bool Card1::debug2 = false;
REGISTER_MINION_CARD_CLASS(1, Card1)

class Card2 : MinionCardBase
{
public:
	static void BattleCry(FlowControl::Context::BattleCry & context)
	{
		debug1 = true;

		state::CardRef target = context.GetBattleCryTarget();

	}

	static void AfterSummoned(FlowControl::Context::AfterSummoned & context)
	{
		debug2 = true;
		CardRef self = context.card_ref_;

		context.state_.event_mgr.PushBack<state::Events::EventTypes::OnTakeDamage>(
			[self](auto& controller, auto& context) {
			if (context.card_ref_ != self) return;
			if (context.damage_ > 0) {
				context.damage_ = 1;
			}
		});

		Manipulate(context).Card(context.card_ref_).Aura().Set(222);
	}

	static bool debug1;
	static bool debug2;
};
bool Card2::debug1 = false;
bool Card2::debug2 = false;
REGISTER_MINION_CARD_CLASS(2, Card2)

class Card2_Enchant1
{
public:
	static constexpr EnchantmentTiers tier = kEnchantmentAura;

	Card2_Enchant1()
		: apply_functor([](auto& card) {
		card.SetCost(0);
	}), after_added_callback(AfterAddedCallback)
	{}

private:
	static void AfterAddedCallback(FlowControl::Context::EnchantmentAfterAdded & context)
	{
		std::cout << "HHHH" << std::endl;
	}

public:
	state::Cards::Enchantments::ApplyFunctor apply_functor;
	state::Cards::Enchantments::AfterAddedCallback *after_added_callback;
};

class Card2_Aura
{
public:
	typedef Card2_Enchant1 EnchantmentType;

	static void GetTargets(FlowControl::Context::AuraGetTargets & context)
	{
		context.targets_.insert(context.card_ref_);
	}

	static void ApplyOn(FlowControl::Context::AuraApplyOn & context)
	{
		context.enchant_id_ = FlowControl::Manipulate(context.state_, context.flow_context_).Card(context.card_ref_).Enchant().Add(EnchantmentType());
	}

	static void RemoveFrom(FlowControl::Context::AuraRemoveFrom & context)
	{
		FlowControl::Manipulate(context.state_, context.flow_context_).Card(context.card_ref_).Enchant().Remove<EnchantmentType>(context.enchant_id_);
	}
};
REGISTER_AURA_CLASS(222, Card2_Aura)

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
	c1.damaged = 0;
	c1.enchanted_states.player = player;
	c1.enchanted_states.zone = state::kCardZoneHand;
	c1.enchanted_states.cost = 5;
	c1.enchanted_states.max_hp = 2;
	c1.enchanted_states.attack = 7;
	return c1;
}

static state::Cards::RawCard GetCard2(state::PlayerIdentifier player, int zone_pos)
{
	state::Cards::RawCard c1;
	c1.card_type = state::kCardTypeMinion;
	c1.card_id = 2;
	c1.zone_position = zone_pos;
	c1.damaged = 0;
	c1.enchanted_states.player = player;
	c1.enchanted_states.zone = state::kCardZoneHand;
	c1.enchanted_states.cost = 1;
	c1.enchanted_states.attack = 3;
	c1.enchanted_states.max_hp = 10;
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

static void MakeDeck(state::State & state, FlowControl::FlowContext & flow_context, state::PlayerIdentifier player)
{
	CardRef r1 = state.mgr.PushBack(state, flow_context, state::Cards::Card(GetCard3(player, state.board.Get(player).deck_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneDeck, 0);

	CardRef r2 = state.mgr.PushBack(state, flow_context, state::Cards::Card(GetCard3(player, state.board.Get(player).deck_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneDeck, 0);
	CheckZoneAndPosition(state, r2, player, state::kCardZoneDeck, 1);

	CardRef r3 = state.mgr.PushBack(state, flow_context, state::Cards::Card(GetCard3(player, state.board.Get(player).deck_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneDeck, 0);
	CheckZoneAndPosition(state, r2, player, state::kCardZoneDeck, 1);
	CheckZoneAndPosition(state, r3, player, state::kCardZoneDeck, 2);

	CardRef r4 = state.mgr.PushBack(state, flow_context, state::Cards::Card(GetCard3(player, state.board.Get(player).deck_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneDeck, 0);
	CheckZoneAndPosition(state, r2, player, state::kCardZoneDeck, 1);
	CheckZoneAndPosition(state, r3, player, state::kCardZoneDeck, 2);
	CheckZoneAndPosition(state, r4, player, state::kCardZoneDeck, 3);

	CardRef r5 = state.mgr.PushBack(state, flow_context, state::Cards::Card(GetCard3(player, state.board.Get(player).deck_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneDeck, 0);
	CheckZoneAndPosition(state, r2, player, state::kCardZoneDeck, 1);
	CheckZoneAndPosition(state, r3, player, state::kCardZoneDeck, 2);
	CheckZoneAndPosition(state, r4, player, state::kCardZoneDeck, 3);
	CheckZoneAndPosition(state, r5, player, state::kCardZoneDeck, 4);
}

static void MakeHand(state::State & state, FlowControl::FlowContext & flow_context, state::PlayerIdentifier player)
{
	CardRef r1 = state.mgr.PushBack(state, flow_context, state::Cards::Card(GetCard1(player, state.board.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneHand, 0);

	CardRef r2 = state.mgr.PushBack(state, flow_context, state::Cards::Card(GetCard1(player, state.board.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneHand, 0);
	CheckZoneAndPosition(state, r2, player, state::kCardZoneHand, 1);

	CardRef r3 = state.mgr.PushBack(state, flow_context, state::Cards::Card(GetCard1(player, state.board.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneHand, 0);
	CheckZoneAndPosition(state, r2, player, state::kCardZoneHand, 1);
	CheckZoneAndPosition(state, r3, player, state::kCardZoneHand, 2);

	CardRef r4 = state.mgr.PushBack(state, flow_context, state::Cards::Card(GetCard2(player, state.board.Get(player).hand_.Size())));
	CheckZoneAndPosition(state, r1, player, state::kCardZoneHand, 0);
	CheckZoneAndPosition(state, r2, player, state::kCardZoneHand, 1);
	CheckZoneAndPosition(state, r3, player, state::kCardZoneHand, 2);
	CheckZoneAndPosition(state, r4, player, state::kCardZoneHand, 3);

	CardRef r5 = state.mgr.PushBack(state, flow_context, state::Cards::Card(GetCard2(player, state.board.Get(player).hand_.Size())));
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
	ActionParameterGetter action_parameter;
	RandomGenerator random;
	state::State state;
	FlowControl::FlowController controller(state, action_parameter, random);

	{
		state.mgr.PushBack(state, controller.context_, state::Cards::Card(GetHero(state::kPlayerFirst)));
		MakeDeck(state, controller.context_, state::kPlayerFirst);
		MakeHand(state, controller.context_, state::kPlayerFirst);
	}

	{
		state.mgr.PushBack(state, controller.context_, state::Cards::Card(GetHero(state::kPlayerSecond)));
		state.board.Get(state::kPlayerSecond).fatigue_damage_ = 3;
		//MakeDeck(state, controller.context_, state::kPlayerSecond);
		MakeHand(state, controller.context_, state::kPlayerSecond);
	}

	state.current_player = state::kPlayerFirst;
	state.board.Get(state::kPlayerFirst).resource_.SetTotal(8);
	state.board.Get(state::kPlayerFirst).resource_.Refill();

	CardRef r1 = state.GetCurrentPlayer().hand_.Get(2);
	bool triggered = false;
	state::Events::EventTypes::OnMinionPlay::FunctorType on_minion_play_1 = 
		[&triggered] (state::Events::HandlersContainerController & controller, const state::Cards::Card & card) {
		triggered = true;
		//std::cout << "OnMinionPlay event: " << card.GetCardId() << std::endl;
	};
	state.event_mgr.PushBack<state::Events::EventTypes::OnMinionPlay>(on_minion_play_1);

	triggered = false;
	Card1::debug1 = false;
	controller.PlayCard(2);
	assert(triggered);
	assert(Card1::debug1);
	assert(Card1::debug2);
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

	auto state_bak = state;
	{
		CardRef attacker = state.board.Get(state::kPlayerFirst).minions_.Get(0);
		CardRef defender = state.board.Get(state::kPlayerSecond).hero_ref_;
		
		bool debug1 = false;
		state.event_mgr.PushBack<state::Events::EventTypes::BeforeAttack>(
			[&state, &attacker, &defender, &debug1](state::Events::HandlersContainerController & controller, state::State & in_state, CardRef & in_attacker, CardRef & in_defender) -> void {
			assert(&state == &in_state);
			assert(attacker == in_attacker);
			assert(defender == in_defender);
			debug1 = true;
		});

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

		state.event_mgr.PushBack<state::Events::EventTypes::OnTakeDamage>(callback1);

		debug1 = false;
		controller.Attack(attacker, defender);
		assert(debug1);
		assert(debug2);
		assert(state.mgr.Get(attacker).GetDamage() == 0);
		assert(state.mgr.Get(defender).GetDamage() == 8);
	}

	state = state_bak;
	{
		assert(state.board.Get(state::kPlayerSecond).minions_.Size() == 0);
		controller.PlayCard(0);
		assert(state.board.Get(state::kPlayerSecond).minions_.Size() == 1);

		CardRef minion1_ref = state.board.Get(state::kPlayerSecond).minions_.Get(0);
		assert(state.mgr.Get(minion1_ref).GetCardId() == 1);

		CardRef attacker = minion1_ref;
		CardRef defender = state.board.Get(state::kPlayerFirst).minions_.Get(0);

		assert(state.mgr.Get(state.board.Get(state::kPlayerFirst).hero_ref_).GetHP() == 30);
		assert(state.mgr.Get(state.board.Get(state::kPlayerSecond).hero_ref_).GetHP() == 26);
		assert(state.mgr.Get(attacker).GetAttack() == 7);
		assert(state.mgr.Get(attacker).GetDamage() == 0);
		assert(state.mgr.Get(defender).GetAttack() == 3);
		assert(state.mgr.Get(defender).GetDamage() == 0);

		controller.Attack(attacker, defender);

		assert(state.mgr.Get(state.board.Get(state::kPlayerFirst).hero_ref_).GetHP() == 28);
		assert(state.mgr.Get(state.board.Get(state::kPlayerSecond).hero_ref_).GetHP() == 26);
		assert(state.mgr.Get(attacker).GetAttack() == 7);
		assert(state.mgr.Get(attacker).GetDamage() == 3);
		assert(state.mgr.Get(attacker).GetZone() == state::kCardZoneGraveyard);
		assert(state.mgr.Get(defender).GetAttack() == 3);
		assert(state.mgr.Get(defender).GetDamage() == 1);
		assert(state.mgr.Get(defender).GetZone() == state::kCardZonePlay);
	}

	return 0;
}