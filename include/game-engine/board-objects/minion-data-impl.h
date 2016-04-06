#pragma once

#include "game-engine/cards/common.h"
#include "minion-data.h"

namespace GameEngine
{

	inline MinionData::MinionData() : card_id(0), pending_removal(false)
	{

	}

	inline MinionData::MinionData(int card_id, int attack, int hp, int max_hp, int spell_damage)
		: pending_removal(false)
	{
		this->card_id = card_id;

		this->stat.SetAttack(attack);
		this->stat.SetHP(hp);
		this->stat.SetMaxHP(max_hp);
		this->stat.SetSpellDamage(spell_damage);
	}

	inline bool MinionData::operator==(MinionData const& rhs) const
	{
		if (this->card_id != rhs.card_id) return false;

		if (this->stat != rhs.stat) return false;

		if (this->attacked_times != rhs.attacked_times) return false;
		if (this->summoned_this_turn != rhs.summoned_this_turn) return false;

		if (this->pending_removal != rhs.pending_removal) return false;

		if (this->triggers_on_death != rhs.triggers_on_death) return false;

		return true;
	}

	inline bool MinionData::operator!=(MinionData const& rhs) const
	{
		return !(*this == rhs);
	}

	inline std::string MinionData::GetDebugString() const
	{
		std::ostringstream oss;

		if (!this->IsValid()) {
			oss << "[EMPTY]";
		}
		else {
			oss << "[" << this->card_id << "] " << this->stat.GetAttack() << " / " << this->stat.GetHP() << " (max hp = " << this->stat.GetMaxHP() << ")";

			if (this->stat.GetSpellDamage() > 0) oss << " [SPELL=" << this->stat.GetSpellDamage() << "]";

			if (this->stat.IsTaunt()) oss << " [TAUNT]";
			if (this->stat.IsCharge()) oss << " [CHARGE]";
			if (this->stat.IsShield()) oss << " [SHIELD]";
			if (this->stat.IsStealth()) oss << " [STEALTH]";
			if (this->stat.IsForgetful()) oss << " [FORGETFUL:" << this->stat.GetForgetfulCount() << "]";
			if (this->stat.IsFreezeAttacker()) oss << " [FREEZE]";
			if (this->stat.IsFreezed()) oss << " [FREEZED]";
			if (this->stat.IsWindFury()) oss << " [WINDFURY]";
			if (this->stat.IsPoisonous()) oss << " [POISONOUS]";
		}

		return oss.str();
	}

	inline MinionData MinionData::FromCard(Card const & card)
	{
		MinionData ret;
		ret.card_id = card.id;

		ret.stat.SetAttack(card.data.minion.attack);
		ret.stat.SetHP(card.data.minion.hp);
		ret.stat.SetMaxHP(card.data.minion.hp);

		if (card.data.minion.taunt) ret.stat.SetTaunt();
		if (card.data.minion.charge) ret.stat.SetCharge();
		if (card.data.minion.shield) ret.stat.SetShield();
		if (card.data.minion.stealth) ret.stat.SetStealth();
		if (card.data.minion.forgetful) ret.stat.SetForgetful();
		if (card.data.minion.freeze) ret.stat.SetFreezeAttacker();
		if (card.data.minion.windfury) ret.stat.SetWindFury();
		if (card.data.minion.poisonous) ret.stat.SetPoisonous();

		if (card.data.minion.spell_damage > 0) ret.stat.SetSpellDamage(card.data.minion.spell_damage);

		// deathrattles
		auto deathrattle = Cards::CardCallbackManager::GetDeathrattle(card.id);
		if (deathrattle) ret.triggers_on_death.push_back(OnDeathTrigger(deathrattle));

		ret.attacked_times = 0;
		ret.summoned_this_turn = true;

		return ret;
	}
} // namespace GameEngine