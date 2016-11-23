#pragma once

#include "EntitiesManager/EntitiesManager.h"

#include "Manipulators/GeneralManipulator.h"
#include "Manipulators/MinionManipulator.h"
#include "Manipulators/SpellManipulator.h"

Manipulators::GeneralManipulator EntitiesManager::GetGeneralManipulator(const CardRef & id)
{
	Entity::Card& card = cards_.Get(id.id);
	return Manipulators::GeneralManipulator(*this, id, card);
}

Manipulators::MinionManipulator EntitiesManager::GetMinionManipulator(const CardRef & id)
{
	Entity::Card& card = cards_.Get(id.id);
	if (card.GetCardType() != Entity::kCardTypeMinion) throw new std::exception("Card type is not minion");

	return Manipulators::MinionManipulator(*this, id, card);
}

Manipulators::SpellManipulator EntitiesManager::GetSpellManipulator(const CardRef & id)
{
	Entity::Card& card = cards_.Get(id.id);
	if (card.GetCardType() != Entity::kCardTypeSpell) throw new std::exception("Card type is not spell");

	return Manipulators::SpellManipulator(card);
}

template <typename T>
CardRef EntitiesManager::PushBack(State::State & state, T&& card)
{
	CardRef ref = CardRef(cards_.PushBack(std::forward<T>(card)));
	this->GetGeneralManipulator(ref).GetZoneChanger().Add(state);
	return ref;
}