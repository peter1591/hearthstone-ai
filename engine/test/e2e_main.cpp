#include <iostream>
#include <assert.h>

#include "state/Configs.h"
#include "engine/FlowControl/FlowController.h"
#include "engine/FlowControl/FlowController-impl.h"

#include "Cards/PreIndexedCards.h"
#include "Cards/Database.h"

void test2();
void test3();
void test4();

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable: 4702 )
#endif
void test_compile()
{
	return;

	state::State state;

	state.GetZoneChanger<state::kCardZoneGraveyard>(*(state::CardRef*)(nullptr))
		.ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	state.GetZoneChanger<state::kCardZoneHand>(*(state::CardRef*)(nullptr))
		.ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	state.GetZoneChanger<state::kCardZoneInvalid>(*(state::CardRef*)(nullptr))
		.ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	state.GetZoneChanger<state::kCardZoneNewlyCreated>(*(state::CardRef*)(nullptr))
		.ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	state.GetZoneChanger<state::kCardZonePlay>(*(state::CardRef*)(nullptr))
		.ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	state.GetZoneChanger<state::kCardZoneRemoved>(*(state::CardRef*)(nullptr))
		.ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
	state.GetZoneChanger<state::kCardZoneSetASide>(*(state::CardRef*)(nullptr))
		.ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());
}
#ifdef _MSC_VER
#pragma warning( pop )
#endif

int main(void)
{
	static_assert(!state::kOrderHandCardsByCardId); // Need to turn off this setting.

	std::cout << "Reading json file...";
	if (!Cards::Database::GetInstance().Initialize("cards.json")) assert(false);
	Cards::PreIndexedCards::GetInstance().Initialize();
	std::cout << " Done." << std::endl;

	test_compile();

	test2();
	test3();
	test4();

	return 0;
}