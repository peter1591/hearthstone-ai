#pragma once

namespace Cards
{
	struct Card_CS1h_001 : public HeroPowerCardBase<Card_CS1h_001> {
		Card_CS1h_001() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				return TargetsGenerator().SpellTargetable().GetInfo();
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_
					.Character(context.GetTarget())
					.Heal(context.card_ref_, context.card_, 2);
			});
		}
	};
}

REGISTER_CARD(CS1h_001);