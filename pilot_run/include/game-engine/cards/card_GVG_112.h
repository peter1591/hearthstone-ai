#ifndef GAME_ENGINE_CARDS_CARD_GVG_112
#define GAME_ENGINE_CARDS_CARD_GVG_112

DEFINE_CARD_CLASS_START(GVG_112)

// Mogor the Ogre

class Aura : public AuraToAllMinions
{
public:
	Aura(Minion & owner) : AuraToAllMinions(owner) {}

private:
	void AddAuraEnchantmentToMinion(Minion & target_minion)
	{
		constexpr int buff_stat = 1 << MinionStat::FLAG_FORGETFUL;
		auto enchantment = std::make_unique<Enchantment_BuffMinion_C<0, 0, 0, buff_stat, false>>();
		this->AddEnchantment(target_minion, std::move(enchantment));
	}

private: // for comparison
	bool EqualsTo(HookListener const& rhs_base) const { return dynamic_cast<decltype(this)>(&rhs_base) != nullptr; }
	std::size_t GetHash() const { return typeid(*this).hash_code(); }
};

DEFINE_CARD_CLASS_END()

#endif
