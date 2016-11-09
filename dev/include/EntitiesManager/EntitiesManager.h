#pragma once

#include <utility>
#include "CloneableContainers/Vector.h"
#include "Entity/Card.h"
#include "EntitiesManager/CardRef.h"
#include "Manipulators/MinionManipulator.h"
#include "Manipulators/SpellManipulator.h"

class EntitiesManager
{
public:
	typedef CloneableContainers::Vector<Card> ContainerType;

	const Card & Get(const CardRef & id) const
	{
		return cards_.Get(id.id);
	}

	template <typename T>
	CardRef PushBack(T&& card)
	{
		return CardRef(cards_.PushBack(std::forward<T>(card)));
	}

	Manipulators::MinionManipulator GetMinionManipulator(const CardRef & id)
	{
		Card& card = cards_.Get(id.id);
		if (card.GetCardType() != kCardTypeMinion) throw new std::exception("Card type is not minion");

		return Manipulators::MinionManipulator(cards_.Get(id.id));
	}

	Manipulators::SpellManipulator GetSpellManipulator(const CardRef & id)
	{
		Card& card = cards_.Get(id.id);
		if (card.GetCardType() != kCardTypeSpell) throw new std::exception("Card type is not spell");

		return Manipulators::SpellManipulator(cards_.Get(id.id));
	}

private:
	ContainerType cards_;
};