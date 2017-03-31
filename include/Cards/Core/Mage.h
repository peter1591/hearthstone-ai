#pragma once

namespace Cards
{
	struct Card_EX1_277 : SpellCardBase<Card_EX1_277> {
		Card_EX1_277() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay context) {
				int damage = 0;
				context.manipulate_
					.Board()
					.CalculateFinalDamageAmount(context.card_ref_, context.card_, 3, &damage);

				Targets targets = TargetsGenerator().Enemy(context).NotMortallyWounded().GetInfo();
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

	struct Card_EX1_294 : SecretCardBase<Card_EX1_294> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterMinionPlayed::Context context) {
			state::Cards::Card const& self_card = context.manipulate_.Board().GetCard(self);
			if (context.manipulate_.Board().GetCurrentPlayerId() == self_card.GetPlayerIdentifier()) return true;
			context.manipulate_.Secret(self).Remove();

			SummonToPlayerByCopy(context, self_card.GetPlayerIdentifier(), context.card_);
			return false;
		};

		Card_EX1_294() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::AfterMinionPlayed>();
		}
	};
}

REGISTER_CARD(EX1_294)
REGISTER_CARD(EX1_277)