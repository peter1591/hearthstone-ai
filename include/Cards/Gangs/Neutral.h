#pragma once

namespace Cards
{
	struct Card_CFM_807 : MinionCardBase<Card_CFM_807> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterSpell::Context&& context) {
			state::PlayerIdentifier owner = context.state_.GetCard(self).GetPlayerIdentifier();
			if (context.card_.GetPlayerIdentifier() == owner) {
				Manipulate(context).HeroPower(owner).SetUsable();
			}
			return true;
		};

		Card_CFM_807() {
			RegisterEvent<InPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::AfterSpell>();
		}
	};
}

REGISTER_CARD(CFM_807)