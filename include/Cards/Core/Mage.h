#pragma once

namespace Cards
{
	struct Card_EX1_277 : SpellCardBase<Card_EX1_277> {
		Card_EX1_277() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay context) {
				int damage = 0;
				FlowControl::Manipulate(context.state_, context.flow_context_)
					.Board()
					.CalculateFinalDamageAmount(context.card_ref_, context.card_, 3, &damage);

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

	struct Card_EX1_294 : SecretCardBase<Card_EX1_294> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterMinionPlayed::Context context) {
			state::Cards::Card const& self_card = context.state_.GetCard(self);
			if (context.state_.GetCurrentPlayerId() == self_card.GetPlayerIdentifier()) return true;
			FlowControl::Manipulate(context.state_, context.flow_context_)
				.Secret(self)
				.Remove();

			SummonToAllyByCopy(context, context.card_);
			return true;
		};

		Card_EX1_294() {
			RegisterEvent<InPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::AfterMinionPlayed>();
		}
	};
}

REGISTER_CARD(EX1_294)
REGISTER_CARD(EX1_277)