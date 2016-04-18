#ifndef GAME_ENGINE_CARDS_CARD_LOE_051
#define GAME_ENGINE_CARDS_CARD_LOE_051

DEFINE_CARD_CLASS_START(LOE_051)
// Jungle Moonkin

class Aura : public MinionAura
{
public:
	Aura(Minion & minion) : MinionAura(minion) {}

	void AfterAdded(Minion & owner_)
	{
		MinionAura::AfterAdded(owner_);

		this->AddEnchantment(this->GetOwner().GetBoard().player, std::make_unique<Enchantment_BuffPlayer_C<0, 2, false>>());
		this->AddEnchantment(this->GetOwner().GetBoard().opponent, std::make_unique<Enchantment_BuffPlayer_C<0, 2, false>>());
	}

private: // for comparison
	bool EqualsTo(HookListener const& rhs_base) const { return dynamic_cast<decltype(this)>(&rhs_base) != nullptr; }
	std::size_t GetHash() const { return typeid(*this).hash_code(); }
};

DEFINE_CARD_CLASS_END()

#endif
