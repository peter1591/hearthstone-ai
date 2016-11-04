#pragma once

#include "EntitiesManager/Card.h"

namespace Manipulators
{
	class BasicManipulator
	{
	public:
		BasicManipulator(Card& card) : card_(card) {}

		void ChangeCardId(const std::string& new_card_id)
		{
			card_.card_id = new_card_id;
		}

	protected:
		Card& card_;
	};
}