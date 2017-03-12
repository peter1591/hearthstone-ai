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

	FlowControl::Manipulate(state, flow_context).Card(*(state::CardRef*)(nullptr))
		.Zone().WithZone<state::kCardZoneDeck>().ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	FlowControl::Manipulate(state, flow_context).Card(*(state::CardRef*)(nullptr))
		.Zone().WithZone<state::kCardZoneGraveyard>().ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	FlowControl::Manipulate(state, flow_context).Card(*(state::CardRef*)(nullptr))
		.Zone().WithZone<state::kCardZoneHand>().ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	FlowControl::Manipulate(state, flow_context).Card(*(state::CardRef*)(nullptr))
		.Zone().WithZone<state::kCardZoneInvalid>().ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	FlowControl::Manipulate(state, flow_context).Card(*(state::CardRef*)(nullptr))
		.Zone().WithZone<state::kCardZonePlay>().ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	FlowControl::Manipulate(state, flow_context).Card(*(state::CardRef*)(nullptr))
		.Zone().WithZone<state::kCardZoneRemoved>().ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	FlowControl::Manipulate(state, flow_context).Card(*(state::CardRef*)(nullptr))
		.Zone().WithZone<state::kCardZoneSetASide>().ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
}

int main(void)
{
	std::cout << "Reading json file...";
	assert(Cards::Database::GetInstance().LoadJsonFile("../../include/Cards/cards.json"));
	std::cout << " Done." << std::endl;

	test_compile();

	test2();
	test3();

	return 0;
}