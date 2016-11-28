#pragma once

DEFINE_CARD_CLASS_START(CS2_122)

// Raid Leader

class Aura : public AuraToAllMinions
{
public:
	Aura(Minion & owner) : AuraToAllMinions(owner) {}

private:
	bool CheckMinionShouldHaveAuraEnchantment(Minion & minion)
	{
		// only add aura to friendly minions
		if (&this->GetOwner().GetMinions() != &minion.GetMinions()) return false;

		// only add aura to others
		if (&this->GetOwner().GetMinion() == &minion.GetMinion()) return false;

		return true;
	}

	void AddAuraEnchantmentToMinion(Minion & target_minion)
	{
		this->AddEnchantment(target_minion, std::make_unique<Enchantment_BuffMinion_C<1, 0, 0, 0, false>>());
	}

private: // for comparison
	bool EqualsTo(HookListener const& rhs_base) const { return dynamic_cast<decltype(this)>(&rhs_base) != nullptr; }
	std::size_t GetHash() const { return typeid(*this).hash_code(); }
};

DEFINE_CARD_CLASS_END()