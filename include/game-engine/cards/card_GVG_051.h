#pragma once

DEFINE_CARD_CLASS_START(GVG_051)
// Warbot

class Aura : public GameEngine::AuraEnrage
{
public:
	Aura(GameEngine::Minion & owner) : AuraEnrage(owner) {}

private:
	void AddEnrageEnchantment(GameEngine::Minion & aura_owner)
	{
		constexpr int attack_boost = 1;
		constexpr int hp_boost = 0;

		auto enchantment = std::make_unique<Enchantment_BuffMinion_C<attack_boost, hp_boost, 0, 0, false>>();

		aura_owner.enchantments.Add(std::move(enchantment), *this);
	}

private: // for comparison
	bool EqualsTo(GameEngine::HookListener const& rhs_base) const { return dynamic_cast<decltype(this)>(&rhs_base) != nullptr; }
	std::size_t GetHash() const { return typeid(*this).hash_code(); }
};

static void AfterSummoned(GameEngine::MinionIterator summoned_minion)
{
	summoned_minion->auras.Add<Aura>();
}
DEFINE_CARD_CLASS_END()