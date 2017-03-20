#pragma once

namespace Cards
{
	struct Card_CS1h_001 : public HeroPowerCardBase<Card_CS1h_001> {
		Card_CS1h_001() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter context) {
				return TargetsGenerator().SpellTargetable().GetInfo();
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay context) {
				FlowControl::Manipulate(context.state_, context.flow_context_)
					.Character(context.GetTarget())
					.Heal(context.card_ref_, context.card_, 2);
			});
		}
	};
}

REGISTER_CARD(CS1h_001);