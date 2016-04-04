#pragma once

DEFINE_CARD_CLASS_START(EX1_412)
// Raging Worgen

class Aura : public AuraEnrage
{
public:
	Aura(Minion & owner) : AuraEnrage(owner) {}

private:
	void AddEnrageEnchantment(Minion & aura_owner)
	{
		constexpr int attack_boost = 1;
		constexpr int hp_boost = 0;
		constexpr int buff_stat = 1 << MinionStat::FLAG_WINDFURY;

		auto enchantment = std::make_unique<Enchantment_BuffMinion_C<attack_boost, hp_boost, 0, buff_stat, false>>();

		aura_owner.enchantments.Add(std::move(enchantment), *this);
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