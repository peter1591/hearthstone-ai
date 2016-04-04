#ifndef GAME_ENGINE_CARDS_CARD_CS2_222
#define GAME_ENGINE_CARDS_CARD_CS2_222

#include <stdexcept>

DEFINE_CARD_CLASS_START(CS2_222)
// Stormwind Champion

// TODO: add an convenient class for this kind of aura
class Aura : public AuraToAllMinions
{
public:
	Aura(Minion & owner) : AuraToAllMinions(owner) {}

private: // hooks
	void HookAfterMinionAdded(Minion & added_minion)
	{
		AuraToAllMinions::HookAfterMinionAdded(added_minion);

		if (this->CheckMinionShouldHaveAuraEnchantment(added_minion)) {
			this->AddAuraEnchantmentToMinion(added_minion);
		}
	}

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

		target_minion.enchantments.Add(std::move(enchantment), *this);
	}

private: // for comparison
	bool EqualsTo(HookListener const& rhs_base) const { return dynamic_cast<decltype(this)>(&rhs_base) != nullptr; }
	std::size_t GetHash() const { return typeid(*this).hash_code(); }
};

static void AfterSummoned(MinionIterator summoned_minion)
{
	summoned_minion->auras.Add<Aura>();
}

DEFINE_CARD_CLASS_END()
#endif
