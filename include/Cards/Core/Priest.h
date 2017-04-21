#pragma once

// http://www.hearthpwn.com/cards?filter-set=2&filter-class=64&sort=-cost&display=1

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

	struct Card_CS1_130 : public SpellCardBase<Card_CS1_130> {
		Card_CS1_130() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.targets_ = TargetsGenerator(context.player_).SpellTargetable().GetInfo();
				return true;
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

REGISTER_CARD(CS1_130)
REGISTER_CARD(CS1h_001)
