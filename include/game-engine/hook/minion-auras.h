#pragma once

#include <list>
#include <memory>
#include "minion-aura.h"

namespace GameEngine
{
	class Minion;

	class MinionAuras
	{
		friend std::hash<MinionAuras>;

	public:
		MinionAuras(Minion & minion) : minion(minion) {}
		~MinionAuras()
		{
			this->Clear();
		}

		bool operator==(MinionAuras const& rhs) const
		{
			if (this->auras.size() != rhs.auras.size()) return false;

			auto it_lhs = this->auras.begin();
			auto it_rhs = rhs.auras.begin();

			while (true)
			{
				if (it_lhs == this->auras.end()) break;
				if (it_rhs == rhs.auras.end()) break;

				if (**it_lhs != **it_rhs) return false;

				++it_lhs;
				++it_rhs;
			}
			// both iterators should reach end here, since the size is equal

			return true;
		}

		bool operator!=(MinionAuras const& rhs) const { return !(*this == rhs); }

		template <typename Aura, typename... Params>
		void Add(Params&&... params)
		{
			Aura * aura = new Aura(this->minion, params...);
			this->auras.push_back(aura);
			aura->AfterAdded(owner);
		}

		bool Empty() const { return this->auras.empty(); }

		void Clear()
		{
			for (auto aura : this->auras) {
				aura->BeforeRemoved(this->minion);
				delete aura;
			}
			this->auras.clear();
		}

	public: // hooks
		void HookAfterMinionAdded(Minion & aura_owner, Minion & added_minion)
		{
			for (auto & aura : this->auras) aura->HookAfterMinionAdded(aura_owner, added_minion);
		}
		void HookAfterOwnerEnraged(Minion &enraged_aura_owner)
		{
			for (auto & aura : this->auras) aura->HookAfterOwnerEnraged(enraged_aura_owner);
		}
		void HookAfterOwnerUnEnraged(Minion &unenraged_aura_owner)
		{
			for (auto & aura : this->auras) aura->HookAfterOwnerUnEnraged(unenraged_aura_owner);
		}
		void HookAfterMinionDamaged(Minion & minion, int damage) {
			for (auto & aura : this->auras) aura->HookAfterMinionDamaged(minion, damage);
		}

	private:
		Minion & minion;
		std::list<MinionAura *> auras;
	};
} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::MinionAuras> {
		typedef GameEngine::MinionAuras argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			for (auto const& aura : s.auras) {
				GameEngine::hash_combine(result, *aura);
			}

			return result;
		}
	};
}