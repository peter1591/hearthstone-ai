#pragma once

#include <list>
#include <memory>
#include "minion-aura.h"

namespace GameEngine
{
	class Minion;

	class MinionAuras
	{
	public:
		MinionAuras(Minion & minion) : minion(minion) {}
		~MinionAuras()
		{
			this->Clear();
		}

		template <typename Aura, typename... Params>
		void Add(Params&&... params)
		{
			Aura * aura = new Aura(this->minion, params...);
			this->auras.push_back(aura);
		}

		void Clear()
		{
			for (auto aura : this->auras) {
				delete aura;
			}
			this->auras.clear();
		}

	private:
		Minion & minion;
		std::list<MinionAura *> auras;
	};
} // namespace GameEngine