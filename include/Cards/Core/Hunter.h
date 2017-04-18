#pragma once

namespace Cards
{
	struct Card_DS1_185 : public SpellCardBase<Card_DS1_185> {
		Card_DS1_185() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target = false;
				return TargetsGenerator(context.player_).SpellTargetable().GetInfo();
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int spell_damage = context.manipulate_.Board().GetSpellDamage(context.player_);
				state::CardRef target = context.GetTarget();
				if (!target.IsValid()) return;
				context.manipulate_.OnBoardCharacter(target).Damage(context.card_ref_, 2 + spell_damage);
			});
		}
	};
}

REGISTER_CARD(DS1_185)