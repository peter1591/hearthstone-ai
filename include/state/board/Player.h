#pragma once

#include "state/board/Deck.h"
#include "state/board/Graveyard.h"
#include "state/board/Hand.h"
#include "state/board/Minions.h"
#include "state/board/Secrets.h"
#include "state/board/PlayerResource.h"

namespace state
{
	namespace detail { template <CardType TargetCardType, CardZone TargetCardZone> struct PlayerDataStructureMaintainer; }

	namespace board
	{
		class Player
		{
			template <CardType TargetCardType, CardZone TargetCardZone> friend struct state::detail::PlayerDataStructureMaintainer;

		public:
			Player() : fatigue_damage_(0), hero_ref_change_id_(0), prophet_velen_effects_(0) {}

			int GetFatigueDamage() const { return fatigue_damage_; }
			void SetFatigueDamage(int v) { fatigue_damage_ = v; }
			void IncreaseFatigueDamage() { ++fatigue_damage_; }

			PlayerResource const& GetResource() const { return resource_; }
			PlayerResource & GetResource() { return resource_; }

			CardRef GetHeroRef() const { return hero_ref_; }
			int GetHeroRefChangeId() const { return hero_ref_change_id_; }

			CardRef GetHeroPowerRef() const { return hero_power_ref_; }

		private: // restrict access to zone changer
			void SetHeroRef(CardRef ref) {
				hero_ref_ = ref;
				++hero_ref_change_id_;
			}

			void SetHeroPowerRef(CardRef ref) { hero_power_ref_ = ref; }
			void InvalidateHeroPowerRef() { hero_power_ref_.Invalidate(); }

		public:
			Deck deck_;
			Hand hand_;
			Minions minions_;
			Secrets secrets_;
			Graveyard graveyard_;

		private:
			CardRef hero_ref_;
			int hero_ref_change_id_;

			state::CardRef hero_power_ref_;

			PlayerResource resource_;
			int fatigue_damage_;

			int prophet_velen_effects_;
		};
	}
}