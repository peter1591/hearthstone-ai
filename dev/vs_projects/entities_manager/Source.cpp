#include <iostream>
#include "EntitiesManager/EntitiesManager.h"

int main(void)
{
	EntitiesManager mgr;

	Entity::RawCard c1;
	c1.card_type = Entity::kCardTypeMinion;
	c1.card_id = "card_id_1";
	c1.zone = Entity::kCardZoneDeck;
	c1.cost = 5;
	CardRef r1 = mgr.PushBack(Entity::Card(c1));
	std::cout << "t1: " << mgr.Get(r1).GetZone() << std::endl;

	c1.zone = Entity::kCardZoneGraveyard;
	std::cout << "t2: " << mgr.Get(r1).GetZone() << std::endl;

	std::cout << "==" << std::endl;
	mgr.GetMinionManipulator(r1).ChangeZone(Entity::kCardZoneHand);
	std::cout << mgr.Get(r1).GetZone() << std::endl;

	auto mgr2(mgr);
	std::cout << "==" << std::endl;
	std::cout << mgr.Get(r1).GetZone() << std::endl;
	std::cout << mgr2.Get(r1).GetZone() << std::endl;

	mgr2.GetMinionManipulator(r1).ChangeZone(Entity::kCardZonePlay);
	std::cout << "==" << std::endl;
	std::cout << mgr.Get(r1).GetZone() << std::endl;
	std::cout << mgr2.Get(r1).GetZone() << std::endl;

	std::cout << "==" << std::endl;
	mgr2.GetMinionManipulator(r1).SetCost(9);
	std::cout << mgr.Get(r1).GetCost() << std::endl;
	std::cout << mgr2.Get(r1).GetCost() << std::endl;

	return 0;
}