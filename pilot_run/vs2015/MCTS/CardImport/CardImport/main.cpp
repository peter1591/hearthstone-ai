#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>

#include "json/reader.h"

#include "game-engine/game-engine.h"

int main(void)
{
	GameEngine::CardDatabase::GetInstance().ReadFromJsonFile("../../../../database/cards.json");

	// write mapping header
	std::ofstream header_file("../../../../include/game-engine/card-id-map.h");
	for (auto const& map : GameEngine::CardDatabase::GetInstance().GetOriginalIdMap())
	{
		header_file << "#define CARD_ID_" << map.first << " " << map.second << std::endl;
	}
	header_file << "#define CARD_MAX_ID " << GameEngine::CardDatabase::GetInstance().GetAvailableCardId() << std::endl;
	header_file.flush();
}