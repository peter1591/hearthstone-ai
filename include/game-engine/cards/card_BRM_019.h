#ifndef GAME_ENGINE_CARDS_CARD_BRM_019
#define GAME_ENGINE_CARDS_CARD_BRM_019

DEFINE_CARD_CLASS_START(BRM_019)

// Grim Patron

class Aura : public MinionAura
{
public:
	Aura(Minion & minion) : MinionAura(minion) {}

	// TODO: can we pass minion iterator directly?
	void HookAfterMinionDamaged(Minion & minion, int damage)
	{
		(void)damage;

		if (&minion != &this->GetOwner()) return;

		if (minion.GetMinion().stat.GetHP() <= 0) return; // not survives the damage

		auto it_owner = minion.GetMinions().GetIteratorForSpecificMinion(this->GetOwner()); // TODO: add interface 'GetIterator() on minion
		if (it_owner.IsEnd()) throw std::runtime_error("owner vanished");
		Card card = CardDatabase::GetInstance().GetCard(Card_BRM_019::card_id);
		it_owner.GoToNext(); // summon the new patron to the right
		StageHelper::SummonMinion(card, it_owner);
	}

private: // for comparison
	bool EqualsTo(HookListener const& rhs_base) const { return dynamic_cast<Aura const*>(&rhs_base) != nullptr; }
	std::size_t GetHash() const { return typeid(*this).hash_code(); }
};

DEFINE_CARD_CLASS_END()

#endif
