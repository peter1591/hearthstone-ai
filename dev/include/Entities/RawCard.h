#pragma once

#include <string>
#include "Entities/CardType.h"
#include "Entities/CardZone.h"

struct RawCard
{
	CardType card_type;
	std::string card_id;
	CardZone zone;
};