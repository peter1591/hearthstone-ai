#pragma once

namespace Cards
{
	struct Card_EX1_294 : public SecretCardBase<Card_EX1_294> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterMinionPlayed::Context context) {
			state::Cards::Card const& self_card = context.manipulate_.Board().GetCard(self);
			if (context.manipulate_.Board().GetCurrentPlayerId() == self_card.GetPlayerIdentifier()) return true;
			context.manipulate_.Secret(self).Remove();

			SummonToPlayerByCopy(context, self_card.GetPlayerIdentifier(),
				context.manipulate_.Board().GetCard(context.card_ref_));
			return false;
		};

		Card_EX1_294() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::AfterMinionPlayed>();
		}
	};
}

REGISTER_CARD(EX1_294)
