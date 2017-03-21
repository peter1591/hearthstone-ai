#pragma once

namespace Cards
{
	struct Card_EX1_384 : SpellCardBase<Card_EX1_384> {
		Card_EX1_384() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay context) {
				int damage = 0;
				FlowControl::Manipulate(context.state_, context.flow_context_)
					.Board()
					.CalculateFinalDamageAmount(context.card_ref_, context.card_, 8, &damage);

				Targets targets = TargetsGenerator().Enemy(context).NotMortallyWounded().GetInfo();
				for (int i = 0; i < damage; ++i) {
					state::CardRef target = context.flow_context_.GetRandomTarget(context.state_, targets);
					assert(target.IsValid());
					FlowControl::Manipulate(context.state_, context.flow_context_)
						.Character(target)
						.ConductFinalDamage(context.card_ref_, 1);
				}
			});
		}
	};
}

REGISTER_CARD(EX1_384)