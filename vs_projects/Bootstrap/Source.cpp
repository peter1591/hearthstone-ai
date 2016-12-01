#include <algorithm>
#include <fstream>
#include <iostream>
#include "Cards/Database.h"

static void WriteFormattedJson(std::string const& from, std::string const& to)
{
	std::cout << "Write formatted json" << std::endl;

	Json::Reader reader;
	Json::Value json;
	std::ifstream input(from);

	std::cout << "--> Reading file" << std::endl;
	if (reader.parse(input, json, false) == false) throw std::exception("parse failed");

	Json::StyledStreamWriter writer;
	std::ofstream output(to);
	std::cout << "--> Writing file" << std::endl;
	writer.write(output, json);
}

static void WriteMapHeader()
{
	std::cout << "Generating map header" << std::endl;

	Cards::Database::GetInstance().LoadJsonFile("../../include/Cards/cards.json");

	// write mapping header
	std::ofstream header_file("../../include/Cards/id-map.h");
	header_file
		<< "#ifndef BOOTSTRAP_CARDS_ID_MAP_H" << std::endl
		<< "#define BOOTSTRAP_CARDS_ID_MAP_H" << std::endl
		<< "namespace Cards" << std::endl
		<< "{" << std::endl
		<< "	enum CardId" << std::endl
		<< "	{" << std::endl;

	int max_id = -1;
	for (auto const& map : Cards::Database::GetInstance().GetIdMap())
	{
		header_file << "		ID_" << map.first << " = " << map.second << "," << std::endl;
		max_id = std::max(max_id, map.second);
	}

	header_file << "		MAX_ID = " << max_id << std::endl;

	header_file << "	};" << std::endl
		<< "}" << std::endl
		<< "#endif" << std::endl;

	header_file.flush();
}

int main(void)
{
	WriteFormattedJson("../../include/Cards/cards.json", "../../include/Cards/formatted_cards.json");

	WriteMapHeader();

	return 0;
}