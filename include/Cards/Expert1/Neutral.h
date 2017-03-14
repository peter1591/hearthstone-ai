#pragma once

namespace Cards
{
	struct Card_EX1_089 : public MinionCardBase<Card_EX1_089> {
		Card_EX1_089() {
			Battlecry<NoTarget>([](auto context) {
				AnotherPlayer(context).GainEmptyCrystal();
			});
		}
	};

	struct Card_NEW1_038_Enchant : public EnchantmentCardBase {
		static constexpr EnchantmentTiers tier = kEnchantmentTier1;

		Card_NEW1_038_Enchant() {
			apply_functor = [](auto& stats) {
				++stats.attack;
				++stats.max_hp;
			};
		}
	};

	struct Card_NEW1_038 : public MinionCardBase<Card_NEW1_038> {
		template <typename Controller, typename Context>
		static void HandleEvent(Controller&& controller, state::CardRef self, Context&& context) {
			Manipulate(context).Card(self).Enchant().Add(Card_NEW1_038_Enchant());
		};

		Card_NEW1_038() {
			RegisterEvent<InPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::OnTurnEnd>();
		}
	};

	struct Card_EX1_020 : public MinionCardBase<Card_EX1_020> {
		Card_EX1_020() { Shield(); }
	};

	struct Card_CS1_069 : public MinionCardBase<Card_CS1_069> {
		Card_CS1_069() { Taunt(); }
	};
}

REGISTER_MINION_CARD(EX1_089)
REGISTER_MINION_CARD(NEW1_038)
REGISTER_MINION_CARD(EX1_020)
REGISTER_MINION_CARD(CS1_069)
