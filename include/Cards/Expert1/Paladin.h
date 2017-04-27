#pragma once

// http://www.hearthpwn.com/cards?filter-set=3&filter-class=32&sort=-cost&display=1

namespace Cards
{
	struct Card_EX1_363e : public EventHookedEnchantment<Card_EX1_363e> {
		using EventType = state::Events::EventTypes::BeforeAttack;
		static void HandleEvent(EventHookedEnchantmentHandler<Card_EX1_363e> & handler) {
			if (handler.context.attacker_ != handler.card_ref) return;
			handler.context.manipulate_.Hero(handler.aux_data.player).DrawCard();
		}
	};
	struct Card_EX1_363 : public SpellCardBase<Card_EX1_363> {
		Card_EX1_363() {
			onplay_handler.SetSpecifyTargetCallback([](FlowControl::onplay::context::GetSpecifiedTarget const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).Minion().SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardMinion(context.GetTarget()).Enchant().AddEventHooked(Card_EX1_363e());
			});
		}
	};

	struct Card_EX1_132 : public SecretCardBase<Card_EX1_132> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterTakenDamage::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (context.card_ref_ != context.manipulate_.Board().Player(player).GetHeroRef()) return true;
			context.manipulate_.Hero(player.Opposite()).Damage(self, context.damage_);
			context.manipulate_.OnBoardSecret(self).Reveal();
			return false;
		}
		Card_EX1_132() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::AfterTakenDamage>();
		}
	};

	struct Card_EX1_384 : SpellCardBase<Card_EX1_384> {
		Card_EX1_384() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int damage = 0;
				context.manipulate_
					.Board()
					.CalculateFinalDamageAmount(context.card_ref_, 8, &damage);

				Targets targets = TargetsGenerator(context.player_).Enemy().Alive().GetInfo();
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
}

REGISTER_CARD(EX1_384)

REGISTER_CARD(EX1_132)
REGISTER_CARD(EX1_363)
