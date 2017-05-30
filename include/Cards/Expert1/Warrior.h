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
}

REGISTER_CARD(EX1_392)
REGISTER_CARD(EX1_409)
REGISTER_CARD(EX1_410)
REGISTER_CARD(EX1_607)