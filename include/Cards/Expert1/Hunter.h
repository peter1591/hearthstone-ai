#pragma once

namespace Cards
{
	struct Card_EX1_610 : SecretCardBase<Card_EX1_610> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnAttack::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			state::CardRef hero_ref = context.manipulate_.Board().Player(player).GetHeroRef();
			if (context.defender_ != hero_ref) return true;

			int spell_damage = context.manipulate_.Board().GetSpellDamage(player);
			context.manipulate_.Board().Player(player).minions_.ForEach([&](state::CardRef card_ref) {
				context.manipulate_.OnBoardMinion(card_ref).Damage(self, 2 + spell_damage);
			});
			return false;
		};
		Card_EX1_610() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::OnAttack>();
		}
	};

	struct Card_EX1_544 : public SpellCardBase<Card_EX1_544> {
		Card_EX1_544() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Board().Player(context.player_.Opposite()).secrets_.ForEach([&](state::CardRef card_ref) {
					context.manipulate_.OnBoardSecret(card_ref).Remove();
				});
				context.manipulate_.Hero(context.player_).DrawCard();
			});
		}
	};

	struct Card_EX1_611e : public Enchantment<Card_EX1_611e, Cost<2>> {};
	struct Card_EX1_611 : public SecretCardBase<Card_EX1_611> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnAttack::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			state::Cards::Card const& attacker_card = context.manipulate_.GetCard(context.attacker_);
			state::PlayerIdentifier attacker_player = attacker_card.GetPlayerIdentifier();
			if (player == attacker_player) return true;

			if (attacker_card.GetZone() != state::kCardZonePlay) return true;
			if (attacker_card.GetHP() <= 0) return true;
			if (attacker_card.GetPendingDestroy()) return true;

			context.manipulate_.OnBoardMinion(context.attacker_).MoveTo<state::kCardZoneHand>();
			context.manipulate_.Card(context.attacker_).Enchant().Add<Card_EX1_611e>();
			return false;
		};
		Card_EX1_611() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::OnAttack>();
		}
	};

	struct Card_EX1_531 : MinionCardBase<Card_EX1_531> {};
	struct Card_EX1_534 : MinionCardBase<Card_EX1_534> {
		Card_EX1_534() {
			this->deathrattle_handler.Add([](FlowControl::deathrattle::context::Deathrattle context) {
				SummonAt(context, context.player_, context.zone_pos_, Cards::ID_EX1_531);
				SummonAt(context, context.player_, context.zone_pos_, Cards::ID_EX1_531);
			});
		}
	};
}

REGISTER_CARD(EX1_531)
REGISTER_CARD(EX1_534)

REGISTER_CARD(EX1_611)
REGISTER_CARD(EX1_544)
REGISTER_CARD(EX1_610)
