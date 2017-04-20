#pragma once

namespace Cards
{
	struct Card_EX1_360e : Enchantment<Card_EX1_360e, SetAttack<1>> {}; // TODO: why it works without public inherit?
	struct Card_EX1_360 : SpellCardBase<Card_EX1_360> {
		Card_EX1_360() {
			onplay_handler.SetSpecifyTargetCallback([](FlowControl::onplay::context::GetSpecifiedTarget const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).Minion().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				assert(target.IsValid());
				context.manipulate_.OnBoardMinion(target).Enchant().Add<Card_EX1_360e>();
			});
		}
	};
}

REGISTER_CARD(EX1_360)