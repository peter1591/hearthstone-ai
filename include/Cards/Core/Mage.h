#pragma once

// http://www.hearthpwn.com/cards?filter-premium=1&filter-set=2&filter-class=16&sort=-cost&display=1

namespace Cards
{
	struct Card_EX1_277 : SpellCardBase<Card_EX1_277> {
		Card_EX1_277() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int damage = 0;
				context.manipulate_
					.Board()
					.CalculateFinalDamageAmount(context.card_ref_, 3, &damage);

				Targets targets = TargetsGenerator(context.player_).Enemy(context).Alive().GetInfo();
				for (int i = 0; i < damage; ++i) {
					state::CardRef target = context.manipulate_.GetRandomTarget(targets);
					assert(target.IsValid());
					context.manipulate_
						.OnBoardCharacter(target)
						.ConductFinalDamage(context.card_ref_, 1);
				}
			});
		}
	};

	struct Card_CS2_034 : HeroPowerCardBase<Card_CS2_034> {
		Card_CS2_034() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.targets_ = TargetsGenerator(context.player_).SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_
					.OnBoardCharacter(context.GetTarget())
					.Damage(context.card_ref_, 1);
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

	struct Card_CS2_032 : SpellCardBase<Card_CS2_032> {
		Card_CS2_032() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int damage = 4 + context.manipulate_.Board().GetSpellDamage(context.player_);
				context.manipulate_.Board().Player(context.player_.Opposite()).minions_.ForEach([&](state::CardRef minion) {
					context.manipulate_.OnBoardMinion(minion).Damage(context.card_ref_, damage);
				});
			});
		}
	};

	struct Card_CS2_029 : SpellCardBase<Card_CS2_029> {
		Card_CS2_029() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int spell_damage = context.manipulate_.Board().GetSpellDamage(context.player_);
				state::CardRef target = context.GetTarget();
				assert(target.IsValid());
				context.manipulate_.OnBoardCharacter(target).Damage(context.card_ref_, 6 + spell_damage);
			});
		}
	};
}

REGISTER_CARD(CS2_029)
REGISTER_CARD(CS2_032)
REGISTER_CARD(EX1_294)
REGISTER_CARD(CS2_034)
REGISTER_CARD(EX1_277)
