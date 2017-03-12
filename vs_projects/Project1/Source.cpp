#include <iostream>
#include "state/State.h"
#include "state/FlowContext.h"
#include "state/Cards/Card.h"

#include "FlowControl/Manipulators/Helpers/ZoneChanger-impl.h"

template <state::CardType ChangingCardType>
class AAA
{
public:
	template <state::CardZone ChangeToZone,
		typename = std::enable_if_t<FlowControl::Manipulators::Helpers::PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::SpecifyAddPosition == false>>
	void ChangeTo(state::PlayerIdentifier player_identifier)
	{
		std::cout << true << std::endl;
		std::cout << "hello1: " << FlowControl::Manipulators::Helpers::PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::SpecifyAddPosition << std::endl;
	}

	template <state::CardZone ChangeToZone,
		typename = std::enable_if_t<FlowControl::Manipulators::Helpers::PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::SpecifyAddPosition == true>>
		void ChangeTo(state::PlayerIdentifier player_identifier, int pos)
	{
		std::cout << true << std::endl;
		std::cout << "hello2: " << FlowControl::Manipulators::Helpers::PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::SpecifyAddPosition << std::endl;
	}
};

int main(void)
{
	state::State state;
	state::Cards::Card & card(*(state::Cards::Card*)(nullptr));
	state::FlowContext flow_context(*(state::IRandomGenerator*)(nullptr), *(state::ActionParameterWrapper*)(nullptr));

	assert((std::cout << "hello" << std::endl, true));

	//FlowControl::Manipulators::WeaponManipulator(state, flow_context, state::CardRef(), card)
	//	.Zone().ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());

	//AAA<state::kCardTypeWeapon> aaa;
	//aaa.ChangeTo<state::kCardZonePlay>(state::PlayerIdentifier::First());

	return 0;
}