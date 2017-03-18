#pragma once

namespace Cards
{
	struct Card_EX1_350 : MinionCardBase<Card_EX1_350> {
		static void PlayerFlagAuraApply(state::board::Player & player) {
			player.IncreaseProphetVelenEffects();
		}
		static void PlayerFlagAuraRemove(state::board::Player & player) {
			player.DecreaseProphetVelenEffects();
		}
		Card_EX1_350() {
			PlayerFlagAura<AliveWhenInPlay>();
		}
	};
}

REGISTER_MINION_CARD(EX1_350)