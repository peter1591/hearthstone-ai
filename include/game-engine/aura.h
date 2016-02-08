#pragma once

#include <list>
#include "game-engine/board-objects/minion.h"

namespace GameEngine
{
	class Aura
	{
		friend std::hash<Aura>;

	public:
		Aura() {}
		virtual ~Aura() {}

		bool operator==(Aura const& rhs) const { return this->EqualsTo(rhs); }
		bool operator!=(Aura const& rhs) const { return !(*this == rhs); }

	protected:
		virtual bool EqualsTo(Aura const& rhs) const = 0; // this is a pure virtual class (i.e., no member to be compared)
		virtual std::size_t GetHash() const = 0; // this is a pure virtual class (i.e., no member to be hashed)

	public: // hooks
		virtual void UpdateStat(Board const& board, SlotIndex target, BoardObjects::MinionStat & stat) const {}
	};

	class MemoryManagedAura
	{
		friend std::hash<MemoryManagedAura>;

	public:
		// since we support the aura to be allocated from global static data section,
		// the aura should not be able to modified

		static MemoryManagedAura AllocatedFromHeap(Aura const* aura) { return MemoryManagedAura(aura, true); }
		static MemoryManagedAura AllocatedFromStaticData(Aura const* aura) { return MemoryManagedAura(aura, false); }

		bool operator==(MemoryManagedAura const& rhs) const { return *this->ptr == *rhs.ptr; }
		bool operator!=(MemoryManagedAura const& rhs) const { return !(*this == rhs); }

		~MemoryManagedAura() { if (need_delete) delete ptr; }

	private:
		Aura const* ptr;
		bool need_delete;

		MemoryManagedAura(Aura const* ptr, bool need_delete) : ptr(ptr), need_delete(need_delete) {}
	};

	class RegisteredAura
	{
		friend std::hash<RegisteredAura>;

	public:
		typedef std::list<MemoryManagedAura> container_type;

		RegisteredAura() : container(nullptr) {}
		RegisteredAura(container_type & container, container_type::iterator it) : container(&container), it(it) {}

		bool operator==(RegisteredAura const& rhs) const { return *this->it == *rhs.it; }
		bool operator!=(RegisteredAura const& rhs) const { return !(*this == rhs); }

		void Remove() 
		{
			this->it = this->container->erase(this->it); 
		}

	private:
		container_type * container;
		container_type::iterator it;
	};

	class AuraManager
	{
		friend std::hash<AuraManager>;

	public:
		AuraManager();

		bool operator==(AuraManager const& rhs) const { return this->auras == rhs.auras; }
		bool operator!=(AuraManager const& rhs) const { return !(*this == rhs); }

		RegisteredAura Add(MemoryManagedAura && aura)
		{
			auto it = this->auras.insert(this->auras.end(), std::move(aura));
			return RegisteredAura(this->auras, it);
		}

	private:
		std::list<MemoryManagedAura> auras;
	};
}

inline GameEngine::AuraManager::AuraManager()
{

}

namespace std {
	template <> struct hash<GameEngine::Aura> {
		typedef GameEngine::Aura argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			return s.GetHash();
		}
	};

	template <> struct hash<GameEngine::MemoryManagedAura> {
		typedef GameEngine::MemoryManagedAura argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			return std::hash<GameEngine::Aura>()(*s.ptr);
		}
	};

	template <> struct hash<GameEngine::RegisteredAura> {
		typedef GameEngine::RegisteredAura argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			return std::hash<GameEngine::MemoryManagedAura>()(*s.it);
		}
	};

	template <> struct hash<GameEngine::AuraManager> {
		typedef GameEngine::AuraManager argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			for (auto const& aura : s.auras)
			{
				GameEngine::hash_combine(result, aura);
			}

			return result;
		}
	};
}