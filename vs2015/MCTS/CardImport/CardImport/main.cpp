#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>

#include "json/reader.h"

#include "game-engine/card-database.h"

int main(void)
{
	std::ifstream cards_file("../../../../database/cards.json");

	Json::Reader reader;
	Json::Value cards_json;
	if (reader.parse(cards_file, cards_json, false) == false)
	{
		throw std::runtime_error("cannot parse file");
	}

	GameEngine::CardDatabase card_database;
	card_database.ReadFromJson(cards_json);

	// write mapping header
	std::ofstream header_file("../../../../include/game-engine/card-id-map.h");
	for (auto const& map : card_database.GetOriginalIdMap())
	{
		header_file << "#define CARD_ID_" << map.first << " " << map.second << std::endl;
	}
	header_file.flush();
}