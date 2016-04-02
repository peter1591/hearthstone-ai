#pragma once

#include <list>

namespace GameEngine {
	class Board;

	class Minions;
	class MinionData;
	class Minion;

	class Aura
	{
		friend std::hash<Aura>;

	public:
		Aura() {}
		virtual ~Aura() {}

		bool operator==(Aura const& rhs) const { return this->EqualsTo(rhs); }
		bool operator!=(Aura const& rhs) const { return !(*this == rhs); }

	public: // hooks
		virtual void AfterAdded(Minion & owner) {}
		virtual void BeforeRemoved(Minion & owner) {}

		virtual void HookAfterMinionAdded(Minion & aura_owner, Minion & added_minion) {}
		virtual void HookAfterOwnerEnraged(Minion &enraged_aura_owner) {}
		virtual void HookAfterOwnerUnEnraged(Minion &unenraged_aura_owner) {}
		virtual void HookAfterMinionDamaged(Minion & minion, int damage) {}

	protected:
		virtual bool EqualsTo(Aura const& rhs) const = 0; // this is a pure virtual class (i.e., no member to be compared)
		virtual std::size_t GetHash() const = 0; // this is a pure virtual class (i.e., no member to be hashed)
	};

	class Auras
	{
		friend std::hash<Auras>;

	public:
		Auras() {}

		Auras(Auras const& rhs) = delete;
		Auras & operator=(Auras const& rhs) = delete;
		Auras(Auras && rhs) { *this = std::move(rhs); }
		Auras & operator=(Auras && rhs) {
			this->auras = std::move(rhs.auras);
			return *this;
		}

		bool operator==(Auras const& rhs) const
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

		bool operator!=(Auras const& rhs) const { return !(*this == rhs); }

	public:
		void Add(Minion & owner, std::unique_ptr<Aura> && aura)
		{
			auto ref_ptr = aura.get();
			this->auras.push_back(std::move(aura));
			ref_ptr->AfterAdded(owner);
		}

		void Clear(Minion & owner)
		{
			for (auto & aura : this->auras) aura->BeforeRemoved(owner);
			this->auras.clear();
		}

		bool Empty() const { return this->auras.empty(); }

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
		std::list<std::unique_ptr<Aura>> auras;
	};

} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::Aura> {
		typedef GameEngine::Aura argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			return s.GetHash();
		}
	};
	template <> struct hash<GameEngine::Auras> {
		typedef GameEngine::Auras argument_type;
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