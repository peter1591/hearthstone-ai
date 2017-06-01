#pragma once

namespace Cards
{
	struct Card_CFM_807 : MinionCardBase<Card_CFM_807> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterSpellPlayed::Context const& context) {
			state::PlayerIdentifier owner = context.manipulate_.Board().GetCard(self).GetPlayerIdentifier();
			if (context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier() == owner) {
				Manipulate(context).HeroPower(owner).SetUsable();
			}
			return true;
		};

		Card_CFM_807() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::AfterSpellPlayed>();
		}
	};
}

REGISTER_CARD(CFM_807)