#pragma once

namespace Cards
{
	struct Card_EX1_350 : MinionCardBase<Card_EX1_350> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::PrepareDamage::Context&& context) {
			state::PlayerIdentifier owner = context.state_.GetCard(self).GetPlayerIdentifier();
			if (context.source_card_.GetPlayerIdentifier() != owner) return true; // for friendly only
			if (context.source_card_.GetCardType() == state::kCardTypeSpell ||
				context.source_card_.GetCardType() == state::kCardTypeHeroPower) {
				*context.damage *= 2;
			}
			return true;
		};

		Card_EX1_350() {
			RegisterEvent<InPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::PrepareDamage>();
		}
	};
	
	/*struct Card_EX1_350 : MinionCardBase<Card_EX1_350> {
		static void PlayerFlagAuraApply(state::board::Player & player) {
			player.IncreaseProphetVelenEffects();
		}
		static void PlayerFlagAuraRemove(state::board::Player & player) {
			player.DecreaseProphetVelenEffects();
		}
		Card_EX1_350() {
			PlayerFlagAura<AliveWhenInPlay>();
		}
	};*/
}

REGISTER_MINION_CARD(EX1_350)