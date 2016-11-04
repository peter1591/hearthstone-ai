#pragma once

#include <string>
#include "EntitiesManager/Card.h"

class CardManipulator
{
public:
	CardManipulator(Card& card) : card_(card) {}

	void ChangeCardId(const std::string& new_card_id)
	{
		card_.card_id = new_card_id;
	}

private:
	Card & card_;
};