#include <iostream>
#include <assert.h>

#include "Cards/Database.h"

void test2();
void test3();

int main(void)
{
	std::cout << "Reading json file...";
	assert(Cards::Database::GetInstance().LoadJsonFile("../../include/Cards/cards.json"));
	std::cout << " Done." << std::endl;

	test2();
	test3();

	return 0;
}