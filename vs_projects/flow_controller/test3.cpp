#include <assert.h>
#include <iostream>

#include "FlowControl/FlowController.h"
#include "FlowControl/FlowController-impl.h"

class Test3_ActionParameterGetter : public FlowControl::IActionParameterGetter
{
public:
	int GetMinionPutLocation(int min, int max)
	{
		return next_minion_put_location;
	}

	state::CardRef GetSpecifiedTarget(state::State & state, state::CardRef card_ref, const state::Cards::Card & card, std::vector<state::CardRef> const& targets)
	{
		assert(targets.size() == next_specified_target_count);

		if (next_specified_target_idx < 0) return state::CardRef();
		else return targets[next_specified_target_idx];
	}

	int next_minion_put_location;

	int next_specified_target_count;
	int next_specified_target_idx;
};

class Test3_RandomGenerator : public state::IRandomGenerator
{
public:
	Test3_RandomGenerator() :called_times(0), next_rand(0) {}

	int Get(int exclusive_max)
	{
		++called_times;
		return next_rand;
	}

	size_t Get(size_t exclusive_max) { return (size_t)Get((int)exclusive_max); }

	int Get(int min, int max)
	{
		++called_times;
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

static state::Cards::Card CreateDeckCard(Cards::CardId id, state::State & state, state::PlayerIdentifier player)
{
	state::Cards::CardData raw_card = Cards::CardDispatcher::CreateInstance(id);
	raw_card.enchanted_states.player = player;
	raw_card.zone = state::kCardZoneNewlyCreated;
	raw_card.enchantment_handler.SetOriginalStates(raw_card.enchanted_states);

	return state::Cards::Card(raw_card);
}

static state::CardRef PushBackDeckCard(Cards::CardId id, FlowControl::FlowContext & flow_context, state::State & state, state::PlayerIdentifier player)
{
	int deck_count = (int)state.GetBoard().Get(player).deck_.Size();

	((Test3_RandomGenerator&)(flow_context.GetRandom())).next_rand = deck_count;
	((Test3_RandomGenerator&)(flow_context.GetRandom())).called_times = 0;

	auto ref = state.AddCard(CreateDeckCard(id, state, player));
	state.GetZoneChanger<state::kCardZoneNewlyCreated>(flow_context.GetRandom(), ref)
		.ChangeTo<state::kCardZoneDeck>(player);

	if (deck_count > 0) assert(((Test3_RandomGenerator&)(flow_context.GetRandom())).called_times > 0);
	++deck_count;

	assert(state.GetBoard().Get(player).deck_.Size() == deck_count);
	assert(state.GetCardsManager().Get(ref).GetCardId() == id);
	assert(state.GetCardsManager().Get(ref).GetPlayerIdentifier() == player);
	assert(state.GetCardsManager().Get(ref).GetZone() == state::kCardZoneDeck);

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
	state::Cards::CardData raw_card = Cards::CardDispatcher::CreateInstance(id);
	assert(raw_card.card_type == type);

	raw_card.enchanted_states.player = player;
	raw_card.zone = state::kCardZoneNewlyCreated;
	raw_card.enchantment_handler.SetOriginalStates(raw_card.enchanted_states);

	return state::Cards::Card(raw_card);
}

static state::CardRef AddHandCard(Cards::CardId id, state::CardType type, FlowControl::FlowContext & flow_context, state::State & state, state::PlayerIdentifier player)
{
	int hand_count = (int)state.GetBoard().Get(player).hand_.Size();

	auto ref = state.AddCard(CreateHandCard(id, type, state, player));
	state.GetZoneChanger<state::kCardZoneNewlyCreated>(flow_context.GetRandom(), ref)
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
	AddHandCard(Cards::ID_CS2_141, state::kCardTypeMinion, flow_context, state, player);
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

	state.GetZoneChanger<state::kCardZoneNewlyCreated>(flow_context.GetRandom(), ref)
		.ChangeTo<state::kCardZonePlay>(player);


	auto hero_power = Cards::CardDispatcher::CreateInstance(Cards::ID_CS1h_001);
	assert(hero_power.card_type == state::kCardTypeHeroPower);
	hero_power.zone = state::kCardZoneNewlyCreated;
	ref = state.AddCard(state::Cards::Card(hero_power));
	state.GetZoneChanger<state::kCardZoneNewlyCreated>(flow_context.GetRandom(), ref)
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
	// TODO: armor
	assert(hero.GetAttack() == attack);
}

void test3()
{
	Test3_ActionParameterGetter parameter_getter;
	Test3_RandomGenerator random;
	state::State state;
	FlowControl::FlowContext flow_context(random, parameter_getter);

	FlowControl::FlowController controller(state, flow_context);

	MakeHero(state, flow_context, state::PlayerIdentifier::First());
	MakeDeck(state, flow_context, state::PlayerIdentifier::First());
	MakeHand(state, flow_context, state::PlayerIdentifier::First());

	MakeHero(state, flow_context, state::PlayerIdentifier::Second());
	MakeDeck(state, flow_context, state::PlayerIdentifier::Second());
	MakeHand(state, flow_context, state::PlayerIdentifier::Second());

	state.GetMutableCurrentPlayerId().SetFirst();
	state.GetBoard().GetFirst().GetResource().SetTotal(4);
	state.GetBoard().GetFirst().GetResource().Refill();
	state.GetBoard().GetSecond().GetResource().SetTotal(4);

	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 4 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 0, 4 });
	CheckMinions(state, state::PlayerIdentifier::First(), {});
	CheckMinions(state, state::PlayerIdentifier::Second(), {});
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 1);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

	parameter_getter.next_specified_target_count = 2;
	parameter_getter.next_specified_target_idx = 0;
	if (controller.PlayCard(0) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 29, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 4 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 0, 4 });
	CheckMinions(state, state::PlayerIdentifier::First(), { {2, 2, 2} });
	CheckMinions(state, state::PlayerIdentifier::Second(), {});
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 0);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 1);

	PushBackDeckCard(Cards::ID_CS2_122, flow_context, state, state::PlayerIdentifier::Second());

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

	PushBackDeckCard(Cards::ID_CS2_122, flow_context, state, state::PlayerIdentifier::First());

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

	AddHandCard(Cards::ID_EX1_506, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::First());
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

	state.GetBoard().Get(state::PlayerIdentifier::First()).GetResource().Refill();
	AddHandCard(Cards::ID_EX1_019, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::First());
	parameter_getter.next_minion_put_location = 0;
	parameter_getter.next_specified_target_count = 4;
	parameter_getter.next_specified_target_idx = 2;
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
		auto flow_context2 = flow_context;
		FlowControl::FlowController controller2(state2, flow_context2);
		if (controller2.Attack(
			state2.GetBoard().Get(state::PlayerIdentifier::First()).minions_.Get(3),
			state2.GetBoard().Get(state::PlayerIdentifier::Second()).GetHeroRef()
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

	AddHandCard(Cards::ID_CS2_124, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::Second());
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

	state.GetBoard().Get(state::PlayerIdentifier::Second()).GetResource().Refill();
	AddHandCard(Cards::ID_DS1_055, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::Second());
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

	state.GetBoard().Get(state::PlayerIdentifier::Second()).GetResource().Refill();
	AddHandCard(Cards::ID_CS2_226, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::Second());
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

	state.GetBoard().Get(state::PlayerIdentifier::Second()).GetResource().Refill();
	AddHandCard(Cards::ID_EX1_399, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::Second());
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

	state.GetBoard().Get(state::PlayerIdentifier::Second()).GetResource().Refill();
	AddHandCard(Cards::ID_CS2_189, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::Second());
	parameter_getter.next_specified_target_count = 8;
	parameter_getter.next_specified_target_idx = 6;
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

	AddHandCard(Cards::ID_CS2_189, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::Second());
	parameter_getter.next_specified_target_count = 9;
	parameter_getter.next_specified_target_idx = 7;
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

	state.GetBoard().Get(state::PlayerIdentifier::Second()).GetResource().Refill();
	AddHandCard(Cards::ID_EX1_593, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::Second());
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

	AddHandCard(Cards::ID_CS2_222, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::Second());
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

	if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 7 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 0, 7 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 3, 2, 2 },{ 2, 1, 1 },{ 2, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 2, 2 },{ 6, 6, 6 },{ 5, 5, 5 },{ 2, 2, 2 },{ 7, 7, 7 },{ 9, 6, 8 },{ 5, 6, 6 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 3);

	if (controller.Attack(state.GetBoard().GetFirst().minions_.Get(1),
		state.GetBoard().GetSecond().minions_.Get(2)) != FlowControl::kResultNotDetermined) assert(false);
	if (controller.Attack(state.GetBoard().GetFirst().minions_.Get(1),
		state.GetBoard().GetSecond().minions_.Get(2)) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 7 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 0, 7 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 3, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 2, 2 },{ 6, 6, 6 }, {5, 1, 5}, { 2, 2, 2 },{ 7, 7, 7 },{ 9, 6, 8 },{ 5, 6, 6 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 3);

	AddHandCard(Cards::ID_CS2_203, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::First());
	parameter_getter.next_minion_put_location = 0;
	parameter_getter.next_specified_target_count = 8;
	parameter_getter.next_specified_target_idx = 2;
	if (controller.PlayCard(2) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 4, 7 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 0, 7 });
	CheckMinions(state, state::PlayerIdentifier::First(), { {2, 1, 1}, { 3, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 },{ 6, 6, 6 },{ 4, 1, 4 },{ 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 3);

	AddHandCard(Cards::ID_CS2_188, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::First());
	parameter_getter.next_minion_put_location = 0;
	parameter_getter.next_specified_target_count = 9;
	parameter_getter.next_specified_target_idx = 1;
	if (controller.PlayCard(2) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 7 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 0, 7 });
	CheckMinions(state, state::PlayerIdentifier::First(), { {1, 1, 1}, { 2, 1, 1 },{ 5, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 },{ 6, 6, 6 },{ 4, 1, 4 },{ 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 3);

	AddHandCard(Cards::ID_CS2_188, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::First());
	parameter_getter.next_minion_put_location = 0;
	parameter_getter.next_specified_target_count = 10;
	parameter_getter.next_specified_target_idx = 2;
	if (controller.PlayCard(2) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 7 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 0, 7 });
	CheckMinions(state, state::PlayerIdentifier::First(), { {1, 1, 1}, { 1, 1, 1 },{ 2, 1, 1 },{ 7, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 },{ 6, 6, 6 },{ 4, 1, 4 },{ 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 3);

	if (controller.Attack(state.GetBoard().GetFirst().minions_.Get(3),
		state.GetBoard().GetSecond().minions_.Get(1))
		!= FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 7 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 0, 7 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 },{ 4, 1, 4 },{ 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 3);

	AddHandCard(Cards::ID_CS2_188, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::First());
	parameter_getter.next_minion_put_location = 0;
	parameter_getter.next_specified_target_count = 9;
	parameter_getter.next_specified_target_idx = 4;
	if (controller.PlayCard(2) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 7 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 0, 7 });
	CheckMinions(state, state::PlayerIdentifier::First(), { {1, 1, 1}, { 1, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 },{ 6, 1, 4 },{ 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 3);

	if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 7 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 8, 8 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 1, 1 },{ 1, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 },{ 4, 1, 4 },{ 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	AddHandCard(Cards::ID_EX1_390, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::Second());
	parameter_getter.next_minion_put_location = 0;
	if (controller.PlayCard(4) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 7 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 5, 8 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 1, 1 },{ 1, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { {2, 3, 3}, { 1, 1, 1 },{ 4, 1, 4 },{ 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 2);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
	if (controller.Attack(state.GetBoard().GetFirst().minions_.Get(0),
		state.GetBoard().GetSecond().minions_.Get(0))
		!= FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 8 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 5, 8 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 1, 1 }, { 1, 1, 1 }, { 2, 1, 1 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 5, 2, 3 },{ 1, 1, 1 },{ 4, 1, 4 },{ 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 3);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	if (controller.Attack(state.GetBoard().GetFirst().minions_.Get(0),
		state.GetBoard().GetSecond().minions_.Get(0))
		!= FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 8 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 5, 8 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 1, 1 },{ 2, 1, 1 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 5, 1, 3 },{ 1, 1, 1 },{ 4, 1, 4 },{ 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 3);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	if (controller.Attack(state.GetBoard().GetFirst().minions_.Get(0),
		state.GetBoard().GetSecond().minions_.Get(0))
		!= FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 8, 8 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 5, 8 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 2, 1, 1 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 },{ 4, 1, 4 },{ 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 3);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	AddHandCard(Cards::ID_EX1_008, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::First());
	parameter_getter.next_minion_put_location = 0;
	if (controller.PlayCard(3) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 30, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 8 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 5, 8 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 1, 1 },{ 2, 1, 1 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 },{ 4, 1, 4 },{ 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 3);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 8 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 9, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 1, 1 },{ 2, 1, 1 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 },{ 4, 1, 4 },{ 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 3);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	if (controller.Attack(state.GetBoard().GetSecond().minions_.Get(1),
		state.GetBoard().GetFirst().minions_.Get(0))
		!= FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 8 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 9, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 1, 1, 1 },{ 2, 1, 1 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 },{ 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 3);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	if (controller.Attack(state.GetBoard().GetSecond().minions_.Get(1),
		state.GetBoard().GetFirst().minions_.Get(0))
		!= FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 26, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 7, 8 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 9, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), {{ 2, 1, 1 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 3);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	// if a 1/1 is buffed to 2/2 by Stormwind Champion then damaged for 1,
	// if it is silenced, it does not temporarily become a 1/1,
	// get buffed by Stormwind Champion again and thus become an undamaged 2/2.
	if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 25, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 9 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 9, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 2, 1, 1 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 3);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	AddHandCard(Cards::ID_CS2_222, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::First());
	parameter_getter.next_minion_put_location = 0;
	if (controller.PlayCard(3) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 25, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 9 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 9, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { {6, 6, 6},  { 3, 2, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 1, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 3);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	if (controller.Attack(state.GetBoard().GetFirst().minions_.Get(1),
		state.GetBoard().GetSecond().minions_.Get(0))
		!= FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 25, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 29, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 9 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 9, 9 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 6, 6, 6 },{ 3, 1, 2} });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 3);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 25, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 9 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 10, 10 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 6, 6, 6 },{ 3, 1, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 3);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	AddHandCard(Cards::ID_CS2_203, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::Second());
	parameter_getter.next_specified_target_count = 5;
	parameter_getter.next_specified_target_idx = 1;
	parameter_getter.next_minion_put_location = 0;
	if (controller.PlayCard(4) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 25, 0, 0);
	CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 2, 9 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 10 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 6, 6, 6 },{ 3, 1, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 }, { 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 3);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	if (controller.EndTurn() != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 23, 0, 0); // next fatigue: 3
	CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 10 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 6, 6, 6 },{ 3, 1, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 3);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	AddHandCard(Cards::ID_EX1_350, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::First());
	parameter_getter.next_minion_put_location = 0;
	if (controller.PlayCard(3) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 23, 0, 0); // next fatigue: 3
	CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 10 });
	CheckMinions(state, state::PlayerIdentifier::First(), { {8, 8, 8}, { 6, 6, 6 },{ 3, 1, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 3);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	state.GetBoard().GetFirst().GetResource().Refill();
	AddHandCard(Cards::ID_EX1_350, state::kCardTypeMinion, flow_context, state, state::PlayerIdentifier::First());
	parameter_getter.next_minion_put_location = 0;
	if (controller.PlayCard(3) != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 23, 0, 0); // next fatigue: 3
	CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 3, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 10 });
	CheckMinions(state, state::PlayerIdentifier::First(), { {8, 8, 8}, { 8, 8, 8 },{ 6, 6, 6 },{ 3, 1, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 3);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	parameter_getter.next_specified_target_count = 10;
	parameter_getter.next_specified_target_idx = 0;
	if (controller.HeroPower() != FlowControl::kResultNotDetermined) assert(false);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0); // next fatigue: 3
	CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 1, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 10 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 8, 8, 8 },{ 8, 8, 8 },{ 6, 6, 6 },{ 3, 1, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 3);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	{
		auto state2 = state;
		auto flow_context2 = flow_context;
		FlowControl::FlowController controller2(state2, flow_context2);
		if (controller2.HeroPower() != FlowControl::kResultInvalid) assert(false);
	}

	state.GetBoard().GetFirst().GetResource().Refill();
	AddHandCard(Cards::ID_EX1_277, state::kCardTypeSpell, flow_context, state, state::PlayerIdentifier::First());
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0); // next fatigue: 3
	CheckHero(state, state::PlayerIdentifier::Second(), 27, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 10, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 10 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 8, 8, 8 },{ 8, 8, 8 },{ 6, 6, 6 },{ 3, 1, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 4);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);

	random.called_times = 0;
	random.next_rand = 0;
	if (controller.PlayCard(3) != FlowControl::kResultNotDetermined) assert(false);
	assert(random.called_times == 12);
	CheckHero(state, state::PlayerIdentifier::First(), 30, 0, 0); // next fatigue: 3
	CheckHero(state, state::PlayerIdentifier::Second(), 15, 0, 0);
	CheckCrystals(state, state::PlayerIdentifier::First(), { 9, 10 });
	CheckCrystals(state, state::PlayerIdentifier::Second(), { 7, 10 });
	CheckMinions(state, state::PlayerIdentifier::First(), { { 8, 8, 8 },{ 8, 8, 8 },{ 6, 6, 6 },{ 3, 1, 2 } });
	CheckMinions(state, state::PlayerIdentifier::Second(), { { 2, 1, 1 },{ 6, 6, 6 },{ 8, 5, 7 },{ 4, 5, 5 } });
	assert(state.GetBoard().Get(state::PlayerIdentifier::First()).hand_.Size() == 3);
	assert(state.GetBoard().Get(state::PlayerIdentifier::Second()).hand_.Size() == 4);
}