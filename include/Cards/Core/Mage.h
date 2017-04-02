#pragma once

namespace Cards
{
	struct Card_CS2_034 : HeroPowerCardBase<Card_CS2_034> {
		Card_CS2_034() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				return TargetsGenerator(context.player_).SpellTargetable().GetInfo();
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_
					.Character(context.GetTarget())
					.Damage(context.card_ref_, 1);
			});
		}
	};

	struct Card_EX1_277 : SpellCardBase<Card_EX1_277> {
		Card_EX1_277() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int damage = 0;
				context.manipulate_
					.Board()
					.CalculateFinalDamageAmount(context.card_ref_, 3, &damage);

				Targets targets = TargetsGenerator(context.player_).Enemy(context).NotMortallyWounded().GetInfo();
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

			SummonToPlayerByCopy(context, self_card.GetPlayerIdentifier(),
				context.manipulate_.Board().GetCard(context.card_ref_));
			return false;
		};

		Card_EX1_294() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::AfterMinionPlayed>();
		}
	};
}

REGISTER_CARD(EX1_294)
REGISTER_CARD(EX1_277)
REGISTER_CARD(CS2_034)
