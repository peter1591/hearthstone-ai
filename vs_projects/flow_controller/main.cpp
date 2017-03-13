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
	state::FlowContext & flow_context = *(state::FlowContext*)(nullptr);

	state.GetZoneChanger(flow_context, *(state::CardRef*)(nullptr))
		.WithZone<state::kCardZoneDeck>().ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	state.GetZoneChanger(flow_context, *(state::CardRef*)(nullptr))
		.WithZone<state::kCardZoneGraveyard>().ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	state.GetZoneChanger(flow_context, *(state::CardRef*)(nullptr))
		.WithZone<state::kCardZoneHand>().ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	state.GetZoneChanger(flow_context, *(state::CardRef*)(nullptr))
		.WithZone<state::kCardZoneInvalid>().ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	state.GetZoneChanger(flow_context, *(state::CardRef*)(nullptr))
		.WithZone<state::kCardZonePlay>().ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	state.GetZoneChanger(flow_context, *(state::CardRef*)(nullptr))
		.WithZone<state::kCardZoneRemoved>().ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	state.GetZoneChanger(flow_context, *(state::CardRef*)(nullptr))
		.WithZone<state::kCardZoneSetASide>().ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
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