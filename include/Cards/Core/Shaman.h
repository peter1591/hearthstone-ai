#pragma once

namespace Cards
{
	struct Card_hexfrog : public MinionCardBase<Card_hexfrog, Taunt> {};
	struct Card_EX1_246 : public SpellCardBase<Card_EX1_246> {
		Card_EX1_246() {
			onplay_handler.SetSpecifyTargetCallback([](FlowControl::onplay::context::GetSpecifiedTarget context) {
				return TargetsGenerator().Minion().SpellTargetable().GetInfo();
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay context) {
				FlowControl::Manipulate(context.state_, context.flow_context_)
					.Minion(context.GetTarget())
					.Transform(CardId::ID_hexfrog);
			});
		}
	};
}

REGISTER_CARD(hexfrog)
REGISTER_CARD(EX1_246)