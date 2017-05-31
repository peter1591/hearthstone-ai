#include <assert.h>
#include <iostream>

#include "FlowControl/FlowController.h"
#include "FlowControl/FlowController-impl.h"

class Test4_ActionParameterGetter : public FlowControl::IActionParameterGetter
{
public:
	int GetMinionPutLocation(int min, int max)
	{
		return next_minion_put_location;
	}

	state::CardRef GetSpecifiedTarget(state::State & state, state::CardRef card_ref, std::vector<state::CardRef> const& targets)
	{
		assert(targets.size() == next_specified_target_count);

		if (next_specified_target_idx < 0) return state::CardRef();
		else return targets[next_specified_target_idx];
	}

	size_t ChooseOne(std::vector<Cards::CardId> const& cards) {
		choose_one_called = true;
		assert(next_choose_one_count == cards.size());
		return next_choose_one_idx;
	}

	int next_minion_put_location;

	int next_specified_target_count;
	int next_specified_target_idx;

	bool choose_one_called;
	int next_choose_one_count;
	int next_choose_one_idx;
};

class Test4_RandomGenerator : public state::IRandomGenerator
{
public:
	Test4_RandomGenerator() :called_times(0), next_rand(0) {}

	int Get(int exclusive_max)
	{
		++called_times;
		assert(next_rand < exclusive_max);
		return next_rand;
	}

	size_t Get(size_t exclusive_max) { return (size_t)Get((int)exclusive_max); }

	int Get(int min, int max)
	{
		++called_times;
		assert(next_rand >= min);
		assert(next_rand <= max);
		return min + next_rand;
	}

public:
	int called_times;
	int next_rand;
};

static void CheckZoneAndPosition(const state::State & state, state::CardRef ref, state::PlayerIdentifier player, state::CardZone zone, int pos)
{
	auto & item = state.GetCardsManager().Get(ref);
	assert(item.GetPlayerIdentifier() == player);
	assert(item.GetZone() == zone);
	assert(item.GetZonePosition() == pos);
}

static void PushBackDeckCard(Cards::CardId id, FlowControl::FlowContext & flow_context, state::State & state, state::PlayerIdentifier player)
{
	int deck_count = (int)state.GetBoard().Get(player).deck_.Size();

	((Test4_RandomGenerator&)(flow_context.GetRandom())).next_rand = deck_count;
	((Test4_RandomGenerator&)(flow_context.GetRandom())).called_times = 0;

	state.GetBoard().Get(player).deck_.ShuffleAdd(id, flow_context.GetRandom());

	if (deck_count > 0) assert(((Test4_RandomGenerator&)(flow_context.GetRandom())).called_times > 0);
	++deck_count;

	assert(state.GetBoard().Get(player).deck_.Size() == deck_count);
}

static state::Cards::Card CreateHandCard(Cards::CardId id, state::State & state, state::PlayerIdentifier player)
{
	state::Cards::CardData raw_card = Cards::CardDispatcher::CreateInstance(id);

	raw_card.enchanted_states.player = player;
	raw_card.zone = state::kCardZoneNewlyCreated;
	raw_card.enchantment_handler.SetOriginalStates(raw_card.enchanted_states);

	return state::Cards::Card(raw_card);
}

static state::CardRef AddHandCard(Cards::CardId id, FlowControl::FlowContext & flow_context, state::State & state, state::PlayerIdentifier player)
{
	int hand_count = (int)state.GetBoard().Get(player).hand_.Size();

	auto ref = state.AddCard(CreateHandCard(id, state, player));
	state.GetZoneChanger<state::kCardZoneNewlyCreated>(FlowControl::Manipulate(state, flow_context), ref)
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

static void MakeHand(state::State & state, FlowControl::FlowContext & flow_context, state::PlayerIdentifier player)
{
	AddHandCard(Cards::ID_CS2_141, flow_context, state, player);
}

static void MakeHero(state::State & state, FlowControl::FlowContext & flow_context, state::PlayerIdentifier player)
{
	state::Cards::CardData raw_card;
	raw_card.card_id = 8;
	raw_card.card_type = state::kCardTypeHero;
	raw_card.zone = state::kCardZoneNewlyCreated;
	raw_card.enchanted_states.max_hp = 30;
	raw_card.enchanted_states.player = player;
	raw_card.enchanted_states.attack = 0;
	raw_card.enchantment_handler.SetOriginalStates(raw_card.enchanted_states);

	state::CardRef ref = state.AddCard(state::Cards::Card(raw_card));

	state.GetZoneChanger<state::kCardZoneNewlyCreated>(FlowControl::Manipulate(state, flow_context), ref)
		.ChangeTo<state::kCardZonePlay>(player);


	auto hero_power = Cards::CardDispatcher::CreateInstance(Cards::ID_CS1h_001);
	assert(hero_power.card_type == state::kCardTypeHeroPower);
	hero_power.zone = state::kCardZoneNewlyCreated;
	ref = state.AddCard(state::Cards::Card(hero_power));
	state.GetZoneChanger<state::kCardZoneNewlyCreated>(FlowControl::Manipulate(state, flow_context), ref)
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
	assert(state.GetBoard().Get(player).GetResource().GetCurrent() == checking.current);
	assert(state.GetBoard().Get(player).GetResource().GetTotal() == checking.total);
}

static void CheckHero(state::State & state, state::PlayerIdentifier player, int hp, int armor, int attack)
{
	auto hero_ref = state.GetBoard().Get(player).GetHeroRef();
	auto const& hero = state.GetCardsManager().Get(hero_ref);

	assert(hero.GetHP() == hp);
	assert(hero.GetArmor() == armor);
	assert(hero.GetAttack() == attack);
}

void test4()
{
	Test4_ActionParameterGetter parameter_getter;
	Test4_RandomGenerator random;
	state::State state;
	FlowControl::FlowContext flow_context(random, parameter_getter);

	FlowControl::FlowController controller(state, flow_context);

	MakeHero(state, flow_context, state::PlayerIdentifier::First());
	MakeHand(state, flow_context, state::PlayerIdentifier::First());

	MakeHero(state, flow_context, state::PlayerIdentifier::Second());
	MakeHand(state, flow_context, state::PlayerIdentifier::Second());

	state.GetMutableCurrentPlayerId().SetFirst();
	state.GetBoard().GetFirst().GetResource().SetTotal(10);
	state.GetBoard().GetFirst().GetResource().Refill();
	state.GetBoard().GetSecond().GetResource().SetTotal(10);
	state.GetBoard().GetSecond().GetResource().Refill();

	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
	CheckMinions(state, state::PlayerIdentifier::First(), {});
	CheckMinions(state, state::PlayerIdentifier::Second(), {});
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	state.SetTurn(1);

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_534, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {6, 5, 5} });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_312, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2,2,2},{2,2,2} });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_534, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 6, 5, 5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CFM_316, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 1;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 6, 5, 5 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_312, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 1,1,1 },{ 1,1,1 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_534, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 6, 5, 5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CFM_316, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 } , { 6, 5, 5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_312, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 },{ 1,1,1 } ,{ 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_534, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 6, 5, 5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CFM_316, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 } ,{ 6, 5, 5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_188, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 2;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {1,1,1}, { 4,2,2 } ,{ 6, 5, 5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_312, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 1,1,1 },{ 1,1,1 },{ 1,1,1 },{ 1,1,1 } ,{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_534, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 6, 5, 5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CFM_316, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 } ,{ 6, 5, 5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_188, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 2;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 },{ 4,2,2 } ,{ 6, 5, 5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().SetFatigueDamage(0);
		state.GetBoard().GetSecond().SetFatigueDamage(0);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 },{ 2,2,2 } ,{ 6, 5, 5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_312, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 2, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 1,1,1 },{ 1,1,1 } ,{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_NEW1_017, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 2, 2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_NEW1_017, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 2, 2 } ,{ 1, 2, 2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_168, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2, 1, 1}, { 1, 2, 2 } ,{ 1, 2, 2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_NEW1_017, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {3, 4, 4}, { 1, 2, 2 } ,{ 1, 2, 2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		FlowControl::Manipulate(state, flow_context)
			.HeroPower(state::PlayerIdentifier::First())
			.ReplaceHeroPower(Cards::ID_CS2_034); // mage hero power

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_029, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 1, 1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 28, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_360, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_222, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 6,6,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_360, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,6,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_222, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 6,6,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_222, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {7,7,7},{7,7,7} });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_360, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 2;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 7,7,7 },{ 2,7,7 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_393, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2, 3, 3} });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		FlowControl::Manipulate(state, flow_context)
			.HeroPower(state::PlayerIdentifier::First())
			.ReplaceHeroPower(Cards::ID_CS2_034); // mage hero power

		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5, 2, 3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_360, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 2, 3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		FlowControl::Manipulate(state, flow_context)
			.HeroPower(state::PlayerIdentifier::First())
			.ReplaceHeroPower(Cards::ID_CS1h_001); // priest hero power

		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 3, 3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		FlowControl::Manipulate(state, flow_context)
			.HeroPower(state::PlayerIdentifier::First())
			.ReplaceHeroPower(Cards::ID_CS2_034); // mage hero power

		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4, 2, 3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_001, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 2, 2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 2, 2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_001, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 2, 2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 2, 2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		FlowControl::Manipulate(state, flow_context)
			.HeroPower(state::PlayerIdentifier::First())
			.ReplaceHeroPower(Cards::ID_CS2_034); // mage hero power

		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 2, 2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		FlowControl::Manipulate(state, flow_context)
			.HeroPower(state::PlayerIdentifier::First())
			.ReplaceHeroPower(Cards::ID_CS1h_001); // priest

		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 2, 2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_001, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 2, 2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 2, 2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		FlowControl::Manipulate(state, flow_context)
			.HeroPower(state::PlayerIdentifier::First())
			.ReplaceHeroPower(Cards::ID_CS2_034); // mage hero power

		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 2, 2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		FlowControl::Manipulate(state, flow_context)
			.HeroPower(state::PlayerIdentifier::First())
			.ReplaceHeroPower(Cards::ID_CS1h_001); // priest

		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3, 2, 2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		FlowControl::Manipulate(state, flow_context)
			.HeroPower(state::PlayerIdentifier::First())
			.ReplaceHeroPower(Cards::ID_CS2_034); // mage hero power

		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3, 1, 2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		FlowControl::Manipulate(state, flow_context)
			.HeroPower(state::PlayerIdentifier::First())
			.ReplaceHeroPower(Cards::ID_CS1h_001); // priest

		state.GetBoard().GetFirst().GetResource().Refill();
		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5, 2, 2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_564, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_specified_target_idx = 0;
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5, 2, 2 }, {5, 2, 2} });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		FlowControl::Manipulate(state, flow_context)
			.HeroPower(state::PlayerIdentifier::First())
			.ReplaceHeroPower(Cards::ID_CS2_034); // mage hero power
		parameter_getter.next_specified_target_count = 4;
		parameter_getter.next_specified_target_idx = 2;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5, 2, 2 },{ 5, 1, 2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		FlowControl::Manipulate(state, flow_context)
			.HeroPower(state::PlayerIdentifier::First())
			.ReplaceHeroPower(Cards::ID_CS1h_001); // priest
		parameter_getter.next_specified_target_count = 4;
		parameter_getter.next_specified_target_idx = 2;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 7, 2, 2 },{ 7, 2, 2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_564, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_509, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_509, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_506, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 1;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 },{2,1,1},{1,1,1}, {4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_508, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 2;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5,2,2 },{ 3,1,1 },{1,1,1}, { 2,1,1 },{ 6,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_080, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_294, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_080, flow_context, state, state::PlayerIdentifier::Second());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 9, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,3,3 },{1,2,2} });
		CheckMinions(state, state::PlayerIdentifier::Second(), { {1,2,2} });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_294, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,4,4 },{ 2,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 2,3,3 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_564, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_minion_put_location = 1;
		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 5, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,4,4 },{ 2,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 2,3,3 },{3,4,4} });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 5, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,4,4 },{ 2,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 2,3,3 },{ 3,4,4 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_564, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 1;
		parameter_getter.next_specified_target_count = 4;
		parameter_getter.next_specified_target_idx = 2;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,4,4 },{ 2,3,3 },{2,3,3} });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 2,3,3 },{ 3,4,4 },{2,3,3} });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_294, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,5,5 },{ 3,4,4 },{ 3,4,4 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,4,4 },{ 4,5,5 },{ 3,4,4 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_146, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_146, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_112, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 28, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_146, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_112, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 28, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().GetHeroRef(),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 23, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 22, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 20, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_146, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_112, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 28, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().GetHeroRef(),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 23, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 22, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().GetHeroRef(),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 17, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 15, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_146, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_112, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 28, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().GetHeroRef(),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 23, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 22, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_146, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 22, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().GetHeroRef(),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 17, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(1),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 15, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_146, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_112, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 28, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().GetHeroRef(),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 23, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 22, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_146, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 22, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 20, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().GetHeroRef(),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 15, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(1),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 13, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_146, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_112, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 28, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().GetHeroRef(),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 23, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 22, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_146, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 22, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_203, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 2;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 22, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 2,1,1 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(1),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_146, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_112, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 28, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().GetHeroRef(),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 23, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 22, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_146, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 22, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_203, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 2;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 22, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 2,1,1 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(1),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 20, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 2,1,1 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(2),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 18, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 2,1,1 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_010, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		FlowControl::Manipulate(state, flow_context)
			.HeroPower(state::PlayerIdentifier::First())
			.ReplaceHeroPower(Cards::ID_CS2_034); // mage hero power

		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {  });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_010, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		FlowControl::Manipulate(state, flow_context)
			.HeroPower(state::PlayerIdentifier::Second())
			.ReplaceHeroPower(Cards::ID_CS2_034); // mage hero power
		parameter_getter.next_specified_target_count = 2;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 28, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 8, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_169, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_169, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 28, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_169, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 28, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_203, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 1;
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 28, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_169, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_203, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 1;
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 28, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_CS2_203, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_019, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_004, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_004, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_004, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_045, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,5,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,5,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_045, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,5,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_203, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2,1,1}, { 4,5,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 4,5,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(1),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 25, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 4,5,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(1),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_NEW1_018, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_CS2_091, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {  });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_NEW1_018, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_059, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2,2,2} });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_059, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2,2,2}, { 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_222, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {6,6,6}, { 3,3,3 },{ 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_059, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {3,3,3}, { 6,6,6 },{ 4,4,4 },{ 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		FlowControl::Manipulate(state, flow_context)
			.HeroPower(state::PlayerIdentifier::First())
			.ReplaceHeroPower(Cards::ID_CS2_034); // mage hero power

		state.GetBoard().GetFirst().GetResource().Refill();
		parameter_getter.next_specified_target_count = 6;
		parameter_getter.next_specified_target_idx = 3;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,3 },{ 6,6,6 },{ 4,3,4 },{ 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_059, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 4;
		parameter_getter.next_specified_target_idx = 2;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {3,3,3}, { 3,3,3 },{ 6,6,6 },{ 4,5,5 },{ 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		// A Chillwind Yeti (4/5 with max 5 Health) is played.
		// Crazed Alchemist gets played on it and it (becomes 5/4 with max 4 Health).
		// 1 damage is done to it (becomes 5/3 with max 4 Health).
		// When it then gets silenced, the Yeti becomes 4/4 with max 5 Health.
		// This is because its maximum Health is increased from 4 to 5 by the Silence effect,
		// so this also causes the current Health to go up from 3 to 4.

		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_CS2_182, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,5,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_059, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2,2,2}, { 5,4,4 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		FlowControl::Manipulate(state, flow_context)
			.HeroPower(state::PlayerIdentifier::First())
			.ReplaceHeroPower(Cards::ID_CS2_034); // mage hero power

		state.GetBoard().GetFirst().GetResource().Refill();
		parameter_getter.next_specified_target_count = 4;
		parameter_getter.next_specified_target_idx = 2;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 5,3,4 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_203, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 2;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2,1,1}, { 2,2,2 },{ 4,4,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_162, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_162, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 },{ 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_162, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 },{4,2,2}, { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_059, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {3,2,2}, { 3,2,2 },{ 4,4,4 },{ 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_NEW1_021, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 0,7,7 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_NEW1_023, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		FlowControl::Manipulate(state, flow_context)
			.HeroPower(state::PlayerIdentifier::First())
			.ReplaceHeroPower(Cards::ID_CS2_034); // mage hero power

		parameter_getter.next_specified_target_count = 2;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_NEW1_019, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_NEW1_019, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_NEW1_019, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_025, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 2;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 21, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 0, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 },{ 2,4,4 },{ 2,1,1 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_534, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 2;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 18, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 },{6,5,5}, { 2,4,4 },{ 2,1,1 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 17, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 },{ 6,5,5 },{ 2,4,4 },{ 2,1,1 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		FlowControl::Manipulate(state, flow_context)
			.HeroPower(state::PlayerIdentifier::Second())
			.ReplaceHeroPower(Cards::ID_CS2_034); // mage hero power
		parameter_getter.next_specified_target_count = 8;
		parameter_getter.next_specified_target_idx = 3;
		if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 17, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 8, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 },{ 6,4,5 },{ 2,4,4 },{ 2,1,1 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_032, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 17, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 1, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_NEW1_019, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_NEW1_019, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_NEW1_019, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_025, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 2;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 21, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 0, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 },{ 2,4,4 },{ 2,1,1 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_534, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 2;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 18, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 },{ 6,5,5 },{ 2,4,4 },{ 2,1,1 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 17, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 },{ 6,5,5 },{ 2,4,4 },{ 2,1,1 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_032, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 17, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 3, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 6,1,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_NEW1_019, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_NEW1_019, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_NEW1_019, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_025, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 2;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 21, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 0, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 },{ 2,4,4 },{ 2,1,1 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_534, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 2;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 18, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 },{ 6,5,5 },{ 2,4,4 },{ 2,1,1 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 17, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 },{ 6,5,5 },{ 2,4,4 },{ 2,1,1 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_029, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_specified_target_count = 8;
		parameter_getter.next_specified_target_idx = 3;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 11, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 },{2,2,2},{2,2,2},{ 2,4,4 },{ 2,1,1 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_100, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 0,4,4 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_032, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 0,4,4 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 2);

		state::CardRef ref = state.GetBoard().GetSecond().hand_.Get(1);
		assert(state.GetCard(ref).GetCardId() == Cards::ID_CS2_032);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 0,4,4 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 2);

		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 3, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {  });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_032, flow_context, state, state::PlayerIdentifier::Second());
		state.GetBoard().GetSecond().GetResource().Refill();
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 3, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_082, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		random.called_times = 3;
		random.next_rand = 1;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_082, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		random.called_times = 3;
		random.next_rand = 1;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 26, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_055, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_032, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_616, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_616, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2,2,2}, { 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_616, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2,2,2}, { 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_203, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2,1,1}, { 2,2,2 },{ 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_203, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 4;
		parameter_getter.next_specified_target_idx = 2;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2,1,1}, { 2,1,1 },{ 2,2,2 },{ 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_203, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 5;
		parameter_getter.next_specified_target_idx = 4;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2,1,1}, {2,1,1}, { 2,1,1 },{ 2,2,2 },{ 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_CS2_203, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 6;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2,1,1}, { 2,1,1 },{ 2,1,1 },{ 2,1,1 },{ 2,2,2 },{ 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_557, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 0,4,4 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 0,4,4 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		random.called_times = 0;
		random.next_rand = 0;
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		assert(random.called_times == 1);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 0,4,4 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_557, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 0,4,4 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 0,4,4 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		random.called_times = 0;
		random.next_rand = 1;
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		assert(random.called_times == 1);
		CheckHero(state, state::PlayerIdentifier::First(), 27, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 0,4,4 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_076, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_076, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2,2,2}, { 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_076, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2,2,2}, { 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 27, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_076, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 27, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2,2,2}, { 2,2,2 },{ 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_076, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 27, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2,2,2}, { 2,2,2 },{ 2,2,2 },{ 2,2,2 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_049, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_049, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 0, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_006, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 0,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 0,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		random.next_rand = 0;
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_006, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 0,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_008, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {1,1,1}, { 0,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 },{ 0,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_170, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 },{ 0,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { {2,3,3} });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_170, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 4, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 },{ 0,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 2,3,3 },{ 2,3,3 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_620, flow_context, state, state::PlayerIdentifier::First());
		random.next_rand = 1;
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 4, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 },{8,8,8} });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 2,3,3 },{ 2,3,3 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 },{ 8,8,8 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 2,3,3 },{ 2,3,3 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetSecond().minions_.Get(0),
			state.GetBoard().GetFirst().minions_.Get(1)
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 2,3,3 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetSecond().minions_.Get(0),
			state.GetBoard().GetFirst().minions_.Get(0)
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 2,2,3 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_597, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,5,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,4,5 },{ 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,4,5 },{ 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,3,5 },{1,1,1},{ 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 27, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,3,5 },{ 1,1,1 },{ 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 27, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 24, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,2,5 },{1,1,1},{ 1,1,1 },{ 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 24, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 24, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,2,5 },{ 1,1,1 },{ 1,1,1 },{ 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 24, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 20, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,5 },{1,1,1},{ 1,1,1 },{ 1,1,1 },{ 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 20, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 20, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,5 },{ 1,1,1 },{ 1,1,1 },{ 1,1,1 },{ 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 20, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 15, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 },{ 1,1,1 },{ 1,1,1 },{ 1,1,1 },{ 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 15, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 15, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 },{ 1,1,1 },{ 1,1,1 },{ 1,1,1 },{ 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 15, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 9, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,1 },{ 1,1,1 },{ 1,1,1 },{ 1,1,1 },{ 1,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_085, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_085, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,3 },{ 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_085, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {3,3,3}, { 3,3,3 },{ 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_085, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {3,3,3}, { 3,3,3 },{ 3,3,3 },{ 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_085, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {3,3,3}, { 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_085, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_085, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_085, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { {3,3,3},{3,3,3} });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_085, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_085, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetSecond().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_085, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,3 },{ 3,3,3 },{ 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 },{ 3,3,3 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_044, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_044, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{3,3,3} });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_044, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 3,3,3 },{4,4,4} });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_044, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 3,3,3 },{ 4,4,4 },{5,5,5} });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_NEW1_022, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_CS2_106, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_NEW1_022, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {3,3,3} });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_CS2_112, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_NEW1_022, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_043, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {4,2,2} });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_043, flow_context, state, state::PlayerIdentifier::First());
		AddHandCard(Cards::ID_EX1_043, flow_context, state, state::PlayerIdentifier::First());
		AddHandCard(Cards::ID_EX1_043, flow_context, state, state::PlayerIdentifier::First());
		AddHandCard(Cards::ID_EX1_043, flow_context, state, state::PlayerIdentifier::First());
		AddHandCard(Cards::ID_EX1_043, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,6,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 5);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_283, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 2;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5,5,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_283, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 2;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5,5,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_067, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 2;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 26, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_067, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 2;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_283, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5,5,5 },{ 4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(1),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_067, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 2;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_283, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5,5,5 },{ 4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5,5,5 },{ 4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(1),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 25, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5,5,5 },{ 4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_067, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 2;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 26, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_283, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 26, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5,5,5 },{ 4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 25, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5,5,5 },{ 4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(1),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_067, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 2;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 26, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		AddHandCard(Cards::ID_EX1_283, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 26, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5,5,5 },{ 4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 25, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5,5,5 },{ 4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 27, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 23, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5,5,5 },{ 4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(1),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 27, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 19, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5,5,5 },{ 4,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_561, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 2;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 15, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 8,8,8 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_572, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,12,12 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		random.called_times = 0;
		random.next_rand = 4;
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		assert(random.called_times == 1);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,12,12 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		random.called_times = 0;
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		assert(random.called_times == 0);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,12,12 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 9,17,17 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		random.called_times = 0;
		random.next_rand = 4;
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		assert(random.called_times == 1);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 9,17,17 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 27, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		random.called_times = 0;
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		assert(random.called_times == 0);
		CheckHero(state, state::PlayerIdentifier::First(), 27, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 24, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_NEW1_030, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 0, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 12,12,12 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_NEW1_030, flow_context, state, state::PlayerIdentifier::First());
		AddHandCard(Cards::ID_NEW1_030, flow_context, state, state::PlayerIdentifier::First());
		AddHandCard(Cards::ID_NEW1_030, flow_context, state, state::PlayerIdentifier::First());
		AddHandCard(Cards::ID_NEW1_030, flow_context, state, state::PlayerIdentifier::First());
		AddHandCard(Cards::ID_NEW1_030, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 0, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 12,12,12 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 5);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		state.GetBoard().GetFirst().GetResource().Refill();
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 0, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 12,12,12 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().GetHeroRef(),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_CS2_005, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 0;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 2, 2);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().GetHeroRef(),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 2, 2);
		CheckHero(state, state::PlayerIdentifier::Second(), 28, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 2, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_DS1_184, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.choose_one_called = false;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		assert(parameter_getter.choose_one_called == false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
		assert(state.GetBoard().GetFirst().deck_.Size() == 0);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		PushBackDeckCard(Cards::ID_DS1_184, flow_context, state, state::PlayerIdentifier::First());

		AddHandCard(Cards::ID_DS1_184, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.choose_one_called = false;
		parameter_getter.next_choose_one_count = 1;
		parameter_getter.next_choose_one_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		assert(parameter_getter.choose_one_called == true);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
		assert(state.GetBoard().GetFirst().deck_.Size() == 0);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		PushBackDeckCard(Cards::ID_DS1_184, flow_context, state, state::PlayerIdentifier::First());
		PushBackDeckCard(Cards::ID_DS1_184, flow_context, state, state::PlayerIdentifier::First());

		AddHandCard(Cards::ID_DS1_184, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.choose_one_called = false;
		parameter_getter.next_choose_one_count = 2;
		parameter_getter.next_choose_one_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		assert(parameter_getter.choose_one_called == true);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
		assert(state.GetBoard().GetFirst().deck_.Size() == 0);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		PushBackDeckCard(Cards::ID_DS1_184, flow_context, state, state::PlayerIdentifier::First());
		PushBackDeckCard(Cards::ID_DS1_184, flow_context, state, state::PlayerIdentifier::First());
		PushBackDeckCard(Cards::ID_DS1_184, flow_context, state, state::PlayerIdentifier::First());

		AddHandCard(Cards::ID_DS1_184, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.choose_one_called = false;
		parameter_getter.next_choose_one_count = 3;
		parameter_getter.next_choose_one_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		assert(parameter_getter.choose_one_called == true);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
		assert(state.GetBoard().GetFirst().deck_.Size() == 0);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		PushBackDeckCard(Cards::ID_DS1_184, flow_context, state, state::PlayerIdentifier::First());
		PushBackDeckCard(Cards::ID_DS1_184, flow_context, state, state::PlayerIdentifier::First());
		PushBackDeckCard(Cards::ID_DS1_184, flow_context, state, state::PlayerIdentifier::First());
		PushBackDeckCard(Cards::ID_DS1_184, flow_context, state, state::PlayerIdentifier::First());
		PushBackDeckCard(Cards::ID_DS1_184, flow_context, state, state::PlayerIdentifier::First());

		AddHandCard(Cards::ID_DS1_184, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.choose_one_called = false;
		parameter_getter.next_choose_one_count = 3;
		parameter_getter.next_choose_one_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		assert(parameter_getter.choose_one_called == true);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
		assert(state.GetBoard().GetFirst().deck_.Size() == 2);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_CS2_033, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,6,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,6,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_033, flow_context, state, state::PlayerIdentifier::Second());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,6,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,6,6 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,6,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,6,6 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().minions_.Get(0)
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,3,6 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,3,6 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetSecond().minions_.Get(0),
			state.GetBoard().GetFirst().minions_.Get(0)
		) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_CS2_033, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,6,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,6,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_033, flow_context, state, state::PlayerIdentifier::Second());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,6,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,6,6 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,6,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,6,6 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().minions_.Get(0)
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,3,6 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,3,6 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 27, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,3,6 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().minions_.Get(0)
		) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_CS2_033, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,6,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,6,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_033, flow_context, state, state::PlayerIdentifier::Second());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,6,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,6,6 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,6,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,6,6 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().minions_.Get(0)
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,3,6 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,3,6 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 27, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,3,6 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 27, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 24, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,3,6 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,3,6 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetSecond().minions_.Get(0),
			state.GetBoard().GetFirst().minions_.Get(0)
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 27, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 24, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { });
		CheckMinions(state, state::PlayerIdentifier::Second(), { });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_CS2_097, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().GetHeroRef(),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 26, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 25, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_097, flow_context, state, state::PlayerIdentifier::Second());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 25, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetSecond().GetHeroRef(),
			state.GetBoard().GetFirst().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_DS1_188, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().GetHeroRef(),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 25, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 24, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_610, flow_context, state, state::PlayerIdentifier::Second());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 24, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 8, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 24, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 8, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().GetHeroRef(),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 19, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 8, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_334, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_NEW1_023, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_NEW1_023, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 }, {3,2,2} });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);

		AddHandCard(Cards::ID_EX1_334, flow_context, state, state::PlayerIdentifier::Second());
		if (controller.PlayCard(1) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_NEW1_023, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_096, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 1;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);

		AddHandCard(Cards::ID_EX1_334, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_choose_one_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 2,1,1 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_NEW1_023, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_096, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);

		AddHandCard(Cards::ID_EX1_334, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_choose_one_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 2,1,1 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 },{ 2,1,1 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_NEW1_023, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_096, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);

		AddHandCard(Cards::ID_EX1_334, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_choose_one_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 2,1,1 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetSecond().minions_.Get(0),
			state.GetBoard().GetFirst().minions_.Get(0)
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_NEW1_023, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_084, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,3,3 },{ 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);

		AddHandCard(Cards::ID_EX1_334, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_choose_one_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,3,3 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetSecond().minions_.Get(0),
			state.GetBoard().GetFirst().minions_.Get(0)
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_NEW1_023, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_084, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,3,3 },{ 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);

		AddHandCard(Cards::ID_EX1_334, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,3,3 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_203, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_minion_put_location = 1;
		parameter_getter.next_specified_target_count = 2;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 3, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 },{2,3,3} });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 2,1,1} });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 3, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 },{ 2,3,3 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 2,1,1 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_NEW1_023, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_084, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,3,3 },{ 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);

		AddHandCard(Cards::ID_EX1_334, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 6, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 3,3,3 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_022, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 2, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 1,1,1 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 2, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { { 1,1,1 } });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_591, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,5,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_621, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 3,1,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_203, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		parameter_getter.next_specified_target_count = 1;
		parameter_getter.next_specified_target_idx = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2,1,1}, { 3,1,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_621, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,1,1 },{ 3,5,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_335, flow_context, state, state::PlayerIdentifier::First());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5,5,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5,5,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_NEW1_034, flow_context, state, state::PlayerIdentifier::Second());
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 5,5,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { {4,2,2} });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetSecond().minions_.Get(0),
			state.GetBoard().GetFirst().minions_.Get(0)
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 1,1,5 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), { });
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_131, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_131, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 6, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {2,2,2},{2,1,1}, { 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_CS2_073, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_specified_target_count = 3;
		parameter_getter.next_specified_target_idx = 1;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 5, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 2,2,2 },{ 6,1,1 },{ 2,2,2 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_137, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 28, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_137, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 28, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_137, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 26, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 25, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 25, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_137, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 28, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_137, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 26, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_137, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 24, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 23, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 23, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 3);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_323, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 15, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().GetHeroRef(),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 15, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 15, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 26, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_323, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 15, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 15, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 1, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_411, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().GetHeroRef(),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 23, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 22, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), {});
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().GetHeroRef(),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultInvalid) assert(false);
	}();

	[state, flow_context, &parameter_getter, &random]() mutable {
		FlowControl::FlowController controller(state, flow_context);

		AddHandCard(Cards::ID_EX1_414, flow_context, state, state::PlayerIdentifier::First());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { {4,9,9} });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetFirst().minions_.Get(0),
			state.GetBoard().GetSecond().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 26, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,9,9 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 25, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,9,9 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		AddHandCard(Cards::ID_EX1_411, flow_context, state, state::PlayerIdentifier::Second());
		parameter_getter.next_minion_put_location = 0;
		if (controller.PlayCard(1) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 25, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 3, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 4,9,9 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetSecond().GetHeroRef(),
			state.GetBoard().GetFirst().minions_.Get(0)
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 21, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 3, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 10,2,9 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 19, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 10,2,9 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetSecond().GetHeroRef(),
			state.GetBoard().GetFirst().GetHeroRef()
		) != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 23, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 19, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 10,2,9 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
		CheckHero(state, state::PlayerIdentifier::First(), 21, 0, 0);
		CheckHero(state, state::PlayerIdentifier::Second(), 16, 0, 0);
		CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
		CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
		CheckMinions(state, state::PlayerIdentifier::First(), { { 10,2,9 } });
		CheckMinions(state, state::PlayerIdentifier::Second(), {});
		assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
		assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

		if (controller.Attack(
			state.GetBoard().GetSecond().GetHeroRef(),
			state.GetBoard().GetFirst().GetHeroRef()
		) != FlowControl::kResultInvalid) assert(false);
	}();
}