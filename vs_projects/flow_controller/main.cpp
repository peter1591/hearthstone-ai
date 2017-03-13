#include <iostream>
#include <assert.h>
#include "FlowControl/FlowController.h"
#include "FlowControl/FlowController-impl.h"

#include "Cards/Database.h"

void test2();
void test3();

void test_compile()
{
	return;

	state::State & state = *(state::State*)(nullptr);
	state::IRandomGenerator & random = *(state::IRandomGenerator*)(nullptr);
	state::FlowContext & flow_context = *(state::FlowContext*)(nullptr);

	state.GetZoneChanger<state::kCardZoneDeck>(random, *(state::CardRef*)(nullptr))
		.ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	state.GetZoneChanger<state::kCardZoneGraveyard>(random, *(state::CardRef*)(nullptr))
		.ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	state.GetZoneChanger<state::kCardZoneHand>(random, *(state::CardRef*)(nullptr))
		.ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	state.GetZoneChanger<state::kCardZoneInvalid>(random, *(state::CardRef*)(nullptr))
		.ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	state.GetZoneChanger<state::kCardZonePlay>(random, *(state::CardRef*)(nullptr))
		.ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	state.GetZoneChanger<state::kCardZoneRemoved>(random, *(state::CardRef*)(nullptr))
		.ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	state.GetZoneChanger<state::kCardZoneSetASide>(random, *(state::CardRef*)(nullptr))
		.ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
}

int main(void)
{
	std::cout << "Reading json file...";
	if (!Cards::Database::GetInstance().LoadJsonFile("../../include/Cards/cards.json")) assert(false);
	std::cout << " Done." << std::endl;

	test_compile();

	test2();
	test3();

	return 0;
}