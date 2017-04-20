#pragma once

namespace Cards
{
	struct Card_CS1h_001 : public HeroPowerCardBase<Card_CS1h_001> {
		Card_CS1h_001() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.targets_ = TargetsGenerator(context.player_).SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_
					.OnBoardCharacter(context.GetTarget())
					.Heal(context.card_ref_, 2);
			});
		}
	};
}

REGISTER_CARD(CS1h_001);