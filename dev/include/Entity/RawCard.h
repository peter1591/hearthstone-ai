#pragma once

#include <string>
#include "Entity/CardType.h"
#include "Entity/CardZone.h"

struct RawCard
{
	CardType card_type;
	std::string card_id;
	CardZone zone;
	int cost;
};