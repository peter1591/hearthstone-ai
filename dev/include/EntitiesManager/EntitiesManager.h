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
	typedef CloneableContainers::Vector<Entity::Card> ContainerType;

	const Entity::Card & Get(const CardRef & id) const
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
		Entity::Card& card = cards_.Get(id.id);
		if (card.GetCardType() != Entity::kCardTypeMinion) throw new std::exception("Card type is not minion");

		return Manipulators::MinionManipulator(*this, card);
	}

	Manipulators::SpellManipulator GetSpellManipulator(const CardRef & id)
	{
		Entity::Card& card = cards_.Get(id.id);
		if (card.GetCardType() != Entity::kCardTypeSpell) throw new std::exception("Card type is not spell");

		return Manipulators::SpellManipulator(card);
	}

private:
	ContainerType cards_;
};

#include "Manipulators/MinionManipulator-impl.h"
#include "Manipulators/Helpers/AuraHelpers-impl.h"