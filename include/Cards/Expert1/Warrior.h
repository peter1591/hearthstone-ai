#pragma once

// http://www.hearthpwn.com/cards?filter-set=3&filter-class=1024&sort=-cost&display=1
// Last finished: Battle Rage

namespace Cards
{
	struct Card_EX1_607e : Enchantment<Card_EX1_607e, Attack<2>> {};
	struct Card_EX1_607 : SpellCardBase<Card_EX1_607> {
		Card_EX1_607() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredTargets(context.player_).Minion().SpellTargetable();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardMinion(context.GetTarget()).Damage(context.card_ref_, 1);
				context.manipulate_.OnBoardMinion(context.GetTarget()).Enchant().Add<Card_EX1_607e>();
			});
		}
	};

	struct Card_EX1_410 : SpellCardBase<Card_EX1_410> {
		Card_EX1_410() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredTargets(context.player_).Minion().SpellTargetable();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef hero_ref = context.manipulate_.Board().Player(context.player_).GetHeroRef();
				int damage = context.manipulate_.GetCard(hero_ref).GetArmor();
				context.manipulate_.OnBoardMinion(context.GetTarget()).Damage(context.card_ref_, damage);
			});
		}
	};

	struct Card_EX1_409e : Enchantment<Card_EX1_409e, Attack<1>, MaxHP<1>> {};
	struct Card_EX1_409 : SpellCardBase<Card_EX1_409> {
		Card_EX1_409() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef weapon_ref = context.manipulate_.Board().Player(context.player_).GetWeaponRef();
				if (!weapon_ref.IsValid()) {
					weapon_ref = context.manipulate_.Board().AddCardById(Cards::ID_EX1_409t, context.player_);
					context.manipulate_.Hero(context.player_).EquipWeapon<state::kCardZoneNewlyCreated>(weapon_ref);
					return;
				}
				context.manipulate_.Weapon(weapon_ref).Enchant().Add<Card_EX1_409e>();
			});
		}
	};

	struct Card_EX1_392 : SpellCardBase<Card_EX1_392> {
		Card_EX1_392() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int draw_count = 0;
				
				auto op = [&](state::CardRef card_ref) {
					if (context.manipulate_.GetCard(card_ref).GetDamage() > 0) ++draw_count;
				};

				op(context.manipulate_.Board().Player(context.player_).GetHeroRef());
				context.manipulate_.Board().Player(context.player_).minions_.ForEach(op);

				for (int i = 0; i < draw_count; ++i) {
					context.manipulate_.Hero(context.player_).DrawCard();
				}
			});
		}
	};

	struct Card_NEW1_036 : SpellCardBase<Card_NEW1_036> {
		Card_NEW1_036() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::PlayerIdentifier player = context.player_;
				context.manipulate_.AddEvent<state::Events::EventTypes::OnTakeDamage>(
					[&](state::Events::EventTypes::OnTakeDamage::Context context) {
					auto const& card = context.manipulate_.GetCard(context.card_ref_);
					if (card.GetCardType() != state::kCardTypeMinion) return true;
					if (card.GetPlayerIdentifier() != player) return true;
					int max_damage = card.GetHP() - 1;
					if (*context.damage_ > max_damage) *context.damage_ = max_damage;
					return true;
				});
			});
		}
	};

	struct Card_CS2_104e : Enchantment<Card_CS2_104e, Attack<3>, MaxHP<3>> {};
	struct Card_CS2_104 : SpellCardBase<Card_CS2_104> {
		Card_CS2_104() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredTargets(context.player_).Minion().Damaged(); // TODO: only spell targetable
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardMinion(context.GetTarget()).Enchant().Add<Card_CS2_104e>();
			});
		}
	};
}

REGISTER_CARD(CS2_104)
REGISTER_CARD(NEW1_036)
REGISTER_CARD(EX1_392)
REGISTER_CARD(EX1_409)
REGISTER_CARD(EX1_410)
REGISTER_CARD(EX1_607)