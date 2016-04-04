#pragma once

namespace GameEngine
{
	// Introduce some attack/hp/taunt/charge/etc. buffs on minion
	// buff_flags are ORed flags for MinionStat::Flag
	template <int attack_boost, int hp_boost, int spell_damage_boost, int buff_flags, bool one_turn>
	class Enchantment_BuffMinion_C : public Enchantment<Minion>
	{
	public:
		Enchantment_BuffMinion_C();
		bool EqualsTo(Enchantment<Minion> const& rhs_base) const;
		std::size_t GetHash() const;

	public:
		void Apply(Minion & minion);
		void Remove(Minion & minion);
		void TurnEnd(Minion & minion, bool & expired);

	private:
#ifdef DEBUG
		bool after_added_called;
#endif
		int actual_attack_boost; // attack cannot be negative
	};
} // namespace GameEngine