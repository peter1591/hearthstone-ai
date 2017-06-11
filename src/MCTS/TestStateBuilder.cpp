#include "TestStateBuilder.h"

class MyRandomGenerator : public state::IRandomGenerator
{
public:
	MyRandomGenerator() :called_times(0), next_rand(0) {}

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

#pragma warning( push )
#pragma warning( disable: 4505 )
static void PushBackDeckCard(Cards::CardId id, state::IRandomGenerator & random, state::State & state, state::PlayerIdentifier player)
{
	int deck_count = (int)state.GetBoard().Get(player).deck_.Size();

	state.GetBoard().Get(player).deck_.ShuffleAdd(id, random);

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

static state::CardRef AddHandCard(Cards::CardId id, state::State & state, state::PlayerIdentifier player)
{
	int hand_count = (int)state.GetBoard().Get(player).hand_.Size();

	auto ref = state.AddCard(CreateHandCard(id, state, player));
	state.GetZoneChanger<state::kCardZoneNewlyCreated>(ref)
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

static void MakeHand(state::State & state, state::PlayerIdentifier player)
{
	//AddHandCard(Cards::ID_CS2_141, state, player);
	AddHandCard(Cards::ID_NEW1_007, state, player);
}

static void MakeHero(state::State & state, state::PlayerIdentifier player)
{
	state::Cards::CardData raw_card;
	raw_card.card_id = (Cards::CardId)8;
	raw_card.card_type = state::kCardTypeHero;
	raw_card.zone = state::kCardZoneNewlyCreated;
	raw_card.enchanted_states.max_hp = 30;
	raw_card.enchanted_states.player = player;
	raw_card.enchanted_states.attack = 0;
	raw_card.enchantment_handler.SetOriginalStates(raw_card.enchanted_states);

	state::CardRef ref = state.AddCard(state::Cards::Card(raw_card));

	state.GetZoneChanger<state::kCardTypeHero, state::kCardZoneNewlyCreated>(ref)
		.ChangeTo<state::kCardZonePlay>(player);


	auto hero_power = Cards::CardDispatcher::CreateInstance(Cards::ID_CS1h_001);
	assert(hero_power.card_type == state::kCardTypeHeroPower);
	hero_power.zone = state::kCardZoneNewlyCreated;
	ref = state.AddCard(state::Cards::Card(hero_power));
	state.GetZoneChanger<state::kCardTypeHeroPower, state::kCardZoneNewlyCreated>(ref)
		.ChangeTo<state::kCardZonePlay>(player);
}
#pragma warning( pop )

state::State TestStateBuilder::GetState()
{
	state::State state;
	MyRandomGenerator my_random;

	MakeHero(state, state::PlayerIdentifier::First());
	MakeHand(state, state::PlayerIdentifier::First());
	PushBackDeckCard(Cards::ID_NEW1_007, my_random, state, state::PlayerIdentifier::First());

	MakeHero(state, state::PlayerIdentifier::Second());
	MakeHand(state, state::PlayerIdentifier::Second());
	PushBackDeckCard(Cards::ID_NEW1_007, my_random, state, state::PlayerIdentifier::Second());

	state.GetMutableCurrentPlayerId().SetFirst();
	state.GetBoard().GetFirst().GetResource().SetTotal(10);
	state.GetBoard().GetFirst().GetResource().Refill();
	state.GetBoard().GetSecond().GetResource().SetTotal(10);
	state.GetBoard().GetSecond().GetResource().Refill();

	return state;
}