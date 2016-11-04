#include <iostream>
#include "EntitiesManager/EntitiesManager.h"

int main(void)
{
	EntitiesManager mgr;

	Card c1;
	c1.card_id = "card_id_1";
	CardRef r1 = mgr.PushBack(c1);
	std::cout << "t1: " << mgr.Get(r1).card_id << std::endl;

	c1.card_id = "card_id_111";
	std::cout << "t2: " << mgr.Get(r1).card_id << std::endl;

	std::cout << "==" << std::endl;
	mgr.GetManipulator(r1).ChangeCardId("card_id_123");
	std::cout << mgr.Get(r1).card_id << std::endl;

	auto mgr2(mgr);
	std::cout << "==" << std::endl;
	std::cout << mgr.Get(r1).card_id << std::endl;
	std::cout << mgr2.Get(r1).card_id << std::endl;

	mgr2.GetManipulator(r1).ChangeCardId("card_id_234");
	std::cout << "==" << std::endl;
	std::cout << mgr.Get(r1).card_id << std::endl;
	std::cout << mgr2.Get(r1).card_id << std::endl;

	return 0;
}