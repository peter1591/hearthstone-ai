#pragma once

#include <list>
#include <memory>

namespace GameEngine
{
	class Minion;
	class MinionAura;

	class MinionAuras
	{
		friend std::hash<MinionAuras>;

	public:
		MinionAuras(Minion & minion);

		~MinionAuras();

		bool operator==(MinionAuras const& rhs) const;
		bool operator!=(MinionAuras const& rhs) const;

		template <typename Aura, typename... Params>
		void Add(Params&&... params);

		bool Empty() const;

		void Clear();

		// destroy all allocated resources without triggering any hooks (since the whole board is going to be destroyed)
		void Destroy();

	public: // hooks
		void HookAfterMinionAdded(Minion & added_minion);
		void HookAfterOwnerEnraged();
		void HookAfterOwnerUnEnraged();
		void HookAfterMinionDamaged(Minion & minion, int damage);
		void HookBeforeMinionTransform(Minion & minion, int new_card_id);
		void HookAfterMinionTransformed(Minion & minion);

	private:
		Minion & minion;
		std::list<MinionAura *> auras;
	};
} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::MinionAuras> {
		typedef GameEngine::MinionAuras argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const;
	};
}