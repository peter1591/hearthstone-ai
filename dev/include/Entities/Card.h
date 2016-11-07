#pragma once

#include <string>
#include "Entities/RawCard.h"

class Card
{
public:
	explicit Card(const RawCard & data) : data_(data) {}

public: // getters and setters
	CardType GetCardType() const { return data_.card_type; }

	const std::string & GetCardId() const { return data_.card_id; }

	const CardZone GetZone() const { return data_.zone; }
	void SetZone(CardZone new_zone)
	{
		data_.zone = new_zone;
		// TODO: triggers?
	}

	int GetCost() const { return data_.cost; }
	void SetCost(int new_cost) { data_.cost = new_cost; }

private:
	RawCard data_;
};