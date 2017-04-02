#pragma once

namespace Cards
{
	struct Card_EX1_350 : MinionCardBase<Card_EX1_350> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::CalculateHealDamageAmount::Context context) {
			state::PlayerIdentifier owner = context.manipulate_.Board().GetCard(self).GetPlayerIdentifier();
			state::Cards::Card const& source_card = context.manipulate_.GetCard(context.source_ref_);
			if (source_card.GetPlayerIdentifier() != owner) return true; // for friendly only
			if (source_card.GetCardType() == state::kCardTypeSpell ||
				source_card.GetCardType() == state::kCardTypeHeroPower)
			{
				*context.amount_ *= 2; // both healing and damaging. Positive for damaging; nagitive for healing
			}
			return true;
		};

		Card_EX1_350() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::CalculateHealDamageAmount>();
		}
	};
	
	/*struct Card_EX1_350 : MinionCardBase<Card_EX1_350> {
		static void FlagAuraApply(state::board::Player & player) {
			player.IncreaseProphetVelenEffects();
		}
		static void FlagAuraRemove(state::board::Player & player) {
			player.DecreaseProphetVelenEffects();
		}
		Card_EX1_350() {
			PlayerFlagAura<AliveWhenInPlay>();
		}
	};*/

	/* Example to register multiple events
	RegisterEvents<
	RegisteredEventType<InPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::PrepareDamage>,
	RegisteredEventType<InPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::PrepareDamage>
	>();
	*/
}

REGISTER_CARD(EX1_350)