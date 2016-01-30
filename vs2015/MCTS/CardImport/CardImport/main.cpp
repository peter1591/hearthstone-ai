#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>

#include "json/reader.h"

#include "game-engine/card-database.h"

int main(void)
{
	GameEngine::CardDatabase card_database;

	card_database.ReadFromJsonFile("../../../../database/cards.json");

	// write mapping header
	std::ofstream header_file("../../../../include/game-engine/card-id-map.h");
	for (auto const& map : card_database.GetOriginalIdMap())
	{
		header_file << "#define CARD_ID_" << map.first << " " << map.second << std::endl;
	}
	header_file.flush();
}