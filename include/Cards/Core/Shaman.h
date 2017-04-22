#pragma once

// http://www.hearthpwn.com/cards?filter-set=2&filter-class=256&sort=-cost&display=1
// Last finished card: Totemic Call

namespace Cards
{
	struct Card_NEW1_009 : public MinionCardBase<Card_NEW1_009> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnTurnEnd::Context context) {
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (owner != context.manipulate_.Board().GetCurrentPlayerId()) return true;

			context.manipulate_.Board().Player(owner).minions_.ForEach([&](state::CardRef card_ref) {
				context.manipulate_.OnBoardMinion(card_ref).Heal(self, 1);
			});
			return true;
		}
		Card_NEW1_009() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::OnTurnEnd>();
		}
	};

	struct Card_CS2_051 : public MinionCardBase<Card_CS2_051, Taunt> {};

	struct Card_CS2_052 : public MinionCardBase<Card_CS2_052, SpellDamage<1>> {};

	struct Card_CS2_049 : public HeroPowerCardBase<Card_CS2_017> {
	private:
		static constexpr std::array<Cards::CardId, 4> basic_totems_{
			Cards::ID_CS2_050,
			Cards::ID_CS2_051,
			Cards::ID_CS2_052,
			Cards::ID_NEW1_009 };
		static std::array<bool, 4> GetTotemExists(FlowControl::Manipulate & manipulate, state::PlayerIdentifier player) {
			std::array<bool, 4> totems_exists{ false, false, false, false };

			manipulate.Board().Player(player).minions_.ForEach([&](state::CardRef card_ref) {
				for (int i = 0; i<basic_totems_.size(); ++i) {
					if (manipulate.GetCard(card_ref).GetCardId() == basic_totems_[i]) {
						totems_exists[i] = true;
					}
				}
			});

			return totems_exists;
		}
	public:
		Card_CS2_049() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				std::array<bool, 4> totems_exists = GetTotemExists(context.manipulate_, context.player_);
				for (auto exist : totems_exists) {
					if (!exist) return true;
				}
				return false;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				std::array<bool, 4> totems_exists = GetTotemExists(context.manipulate_, context.player_);
				std::vector<Cards::CardId> totems_candidates;
				for (int i = 0; i < 4; ++i) {
					if (totems_exists[i]) continue;
					totems_candidates.push_back(basic_totems_[i]);
				}
				if (totems_candidates.empty()) return;
				size_t rand = context.manipulate_.GetRandom().Get(totems_candidates.size());
				Cards::CardId card_id = totems_candidates[rand];
				SummonToRightmost(context.manipulate_, context.player_, card_id);
			});
		}
	};

	struct Card_hexfrog : public MinionCardBase<Card_hexfrog, Taunt> {};
	struct Card_EX1_246 : public SpellCardBase<Card_EX1_246> {
		Card_EX1_246() {
			onplay_handler.SetSpecifyTargetCallback([](FlowControl::onplay::context::GetSpecifiedTarget const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).Minion().SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_
					.OnBoardMinion(context.GetTarget())
					.Transform(CardId::ID_hexfrog);
			});
		}
	};
}

REGISTER_CARD(hexfrog)
REGISTER_CARD(EX1_246)
REGISTER_CARD(CS2_049)
REGISTER_CARD(CS2_051)
REGISTER_CARD(NEW1_009)
