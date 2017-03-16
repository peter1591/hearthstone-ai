#pragma once

#include "state/Cards/Card.h"

namespace FlowControl
{
	namespace detail
	{
		class Resolver;
	}

	namespace Manipulators
	{
		namespace Helpers
		{
			class WeaponRefRemover
			{
				friend class FlowControl::detail::Resolver;

			public:
				WeaponRefRemover(state::State & state, state::Cards::Card & card) :state_(state), card_(card) {}

			private:
				void operator()()
				{
					assert(AssertCheck());
					card_.ClearWeapon();
				}

				bool AssertCheck()
				{
					assert(card_.GetRawData().weapon_ref.IsValid());
					assert(card_.GetCardType() == state::kCardTypeHero);
					assert(card_.GetPlayerIdentifier() ==
						state_.GetCard(card_.GetRawData().weapon_ref).GetPlayerIdentifier());
					return true;
				}

			private:
				state::State & state_;
				state::Cards::Card & card_;
			};
		}
	}
}