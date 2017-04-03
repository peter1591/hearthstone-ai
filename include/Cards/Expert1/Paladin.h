#pragma once

namespace Cards
{
	struct Card_EX1_384 : SpellCardBase<Card_EX1_384> {
		Card_EX1_384() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int damage = 0;
				context.manipulate_
					.Board()
					.CalculateFinalDamageAmount(context.card_ref_, 8, &damage);

				Targets targets = TargetsGenerator(context.player_).Enemy(context).Alive().GetInfo();
				for (int i = 0; i < damage; ++i) {
					state::CardRef target = context.manipulate_.GetRandomTarget(targets);
					assert(target.IsValid());
					context.manipulate_
						.Character(target)
						.ConductFinalDamage(context.card_ref_, 1);
				}
			});
		}
	};
}

REGISTER_CARD(EX1_384)