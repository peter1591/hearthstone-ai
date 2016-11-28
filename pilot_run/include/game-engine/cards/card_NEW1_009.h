#pragma once

DEFINE_CARD_CLASS_START(NEW1_009)

// Healing Totem

class Aura : public MinionAura
{
public:
	Aura(Minion & owner) : MinionAura(owner) {}

public:
	void TurnEnd(bool owner_turn)
	{
		if (owner_turn) {
			// heal 1 for all friendly minions
			for (auto it = this->GetOwner().GetMinions().GetIterator(0); !it.IsEnd(); it.GoToNext())
			{
				it->Heal(1);
			}
		}
	}

private: // for comparison
	bool EqualsTo(HookListener const& rhs_base) const { return dynamic_cast<decltype(this)>(&rhs_base) != nullptr; }
	std::size_t GetHash() const { return typeid(*this).hash_code(); }
};

DEFINE_CARD_CLASS_END()