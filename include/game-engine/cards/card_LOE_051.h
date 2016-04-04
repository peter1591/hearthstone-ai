#ifndef GAME_ENGINE_CARDS_CARD_LOE_051
#define GAME_ENGINE_CARDS_CARD_LOE_051

#include "card-base.h"

namespace GameEngine {
	namespace Cards {

		class Card_LOE_051
		{
		public:
			static constexpr int card_id = CARD_ID_LOE_051;

			// Jungle Moonkin

			class Aura : public GameEngine::MinionAura
			{
			public:
				Aura(GameEngine::Minion & minion) : GameEngine::MinionAura(minion) {}

				void AfterAdded(Minion & owner_)
				{
					MinionAura::AfterAdded(owner_);

					this->GetOwner().GetBoard().player.enchantments.Add(std::make_unique<Enchantment_BuffPlayer_C<2, false>>(), *this);
					this->GetOwner().GetBoard().opponent.enchantments.Add(std::make_unique<Enchantment_BuffPlayer_C<2, false>>(), *this);
				}

			private: // for comparison
				bool EqualsTo(GameEngine::HookListener const& rhs_base) const { return dynamic_cast<decltype(this)>(&rhs_base) != nullptr; }
				std::size_t GetHash() const { return typeid(*this).hash_code(); }
			};

			static void AfterSummoned(GameEngine::MinionIterator summoned_minion)
			{
				summoned_minion->auras.Add<Aura>();
			}
		};

	} // namespace Cards
} // namespace GameEngine

#endif
