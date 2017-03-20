#pragma once

namespace Cards
{
	struct Card_CS1h_001 : public HeroPowerCardBase<Card_CS1h_001> {
		Card_CS1h_001() {
			specified_target_getter = [](Contexts::SpecifiedTargetGetter context) {
				return TargetsGenerator().SpellTargetable().GetInfo();
			};
			spell_handler.SetCallback_DoSpell([](FlowControl::spell::context::DoSpell context) {
				FlowControl::Manipulate(context.state_, context.flow_context_)
					.Character(context.GetTarget())
					.Heal(context.card_ref_, context.card_, 2);
			});
		}
	};
}

REGISTER_CARD(CS1h_001);