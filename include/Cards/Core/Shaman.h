#pragma once

namespace Cards
{
	struct Card_hexfrog : public MinionCardBase<Card_hexfrog, Taunt> {};
	struct Card_EX1_246 : public SpellCardBase<Card_EX1_246> {
		Card_EX1_246() {
			onplay_handler.SetSpecifyTargetCallback([](FlowControl::onplay::context::GetSpecifiedTarget const& context) {
				return TargetsGenerator(context.player_).Minion().SpellTargetable().GetInfo();
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_
					.OnBoardMinion(context.GetTarget())
					.Transform(CardId::ID_hexfrog);
			});
		}
	};
}

REGISTER_CARD(hexfrog)
REGISTER_CARD(EX1_246)