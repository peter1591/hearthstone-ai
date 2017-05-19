#pragma once

// http://www.hearthpwn.com/cards?filter-set=3&filter-class=64&sort=-cost&display=1
// Last finished card: Inner Fire

namespace Cards
{
	struct Card_EX1_621 : SpellCardBase<Card_EX1_621> {
		Card_EX1_621() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				auto op = [&](state::CardRef target) {
					context.manipulate_.OnBoardMinion(target).Heal(context.card_ref_, 4);
				};
				context.manipulate_.Board().FirstPlayer().minions_.ForEach(op);
				context.manipulate_.Board().SecondPlayer().minions_.ForEach(op);
			});
		}
	};

	struct Card_EX1_332 : SpellCardBase<Card_EX1_332> {
		Card_EX1_332() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).Minion().SpellTargetable().GetInfo();
				return true; // return false if card cannot be played
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardMinion(context.GetTarget()).Silence();
			});
		}
	};

	template <int v> struct Card_CS1_129e : EnchantmentCardBase<Card_CS1_129e, SetAttack<v>> {};
	struct Card_CS1_129 : SpellCardBase<Card_CS1_129> {
		Card_CS1_129() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).Minion().SpellTargetable().GetInfo();
				return true; // return false if card cannot be played
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int hp = context.manipulate_.GetCard(context.GetTarget()).GetHP();
				context.manipulate_.OnBoardMinion(context.GetTarget())
					.Enchant().Add<Card_CS1_129e>(hp);
			});
		}
	};

	struct Card_EX1_350 : MinionCardBase<Card_EX1_350> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::CalculateHealDamageAmount::Context context) {
			state::PlayerIdentifier owner = context.manipulate_.Board().GetCard(self).GetPlayerIdentifier();
			state::Cards::Card const& source_card = context.manipulate_.GetCard(context.source_ref_);
			if (source_card.GetPlayerIdentifier() != owner) return true; // for friendly only
			if (source_card.GetCardType() == state::kCardTypeSpell ||
				source_card.GetCardType() == state::kCardTypeHeroPower)
			{
				*context.amount_ *= 2; // both healing and damaging. Positive for damaging; nagitive for healing
			}
			return true;
		};

		Card_EX1_350() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::CalculateHealDamageAmount>();
		}
	};
	
	/* Example to register multiple events
	RegisterEvents<
	RegisteredEventType<InPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::PrepareDamage>,
	RegisteredEventType<InPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::PrepareDamage>
	>();
	*/
}

REGISTER_CARD(EX1_350)

REGISTER_CARD(EX1_129)
REGISTER_CARD(EX1_332)
REGISTER_CARD(EX1_621)