#ifndef GAME_ENGINE_CARDS_CARD_CS2_222
#define GAME_ENGINE_CARDS_CARD_CS2_222

#include <stdexcept>

DEFINE_CARD_CLASS_START(CS2_222)
// Stormwind Champion

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
		constexpr int attack_boost = 1;
		constexpr int hp_boost = 1;

		auto enchantment = std::make_unique<Enchantment_BuffMinion_C<attack_boost, hp_boost, 0, 0, false>>();

		this->AddEnchantment(target_minion, std::move(enchantment));
	}

private: // for comparison
	bool EqualsTo(HookListener const& rhs_base) const { return dynamic_cast<decltype(this)>(&rhs_base) != nullptr; }
	std::size_t GetHash() const { return typeid(*this).hash_code(); }
};

DEFINE_CARD_CLASS_END()
#endif
