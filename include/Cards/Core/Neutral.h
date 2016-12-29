#pragma once

// http://www.hearthpwn.com/cards?filter-set=2&filter-premium=1&filter-class=1&sort=-cost&display=1

namespace Cards
{
	class Card_CS2_189 : public MinionCardBase<Card_CS2_189>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS2_189;

		Card_CS2_189()
		{
			battlecry_target_getter = [] (auto context) {
				SetBattlecryTarget(context, Targets().Targetable());
				return true;
			};
			battlecry = [](auto context) {
				Damage(context).Target(context.flow_context_.battlecry_target_).Amount(1);
			};
		}
	};

	class Card_CS1_042 : public MinionCardBase<Card_CS1_042>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS1_042;

		Card_CS1_042()
		{
			Taunt();
		}
	};

	class Card_CS2_168 : public MinionCardBase<Card_CS2_168>
	{
	public:
		static constexpr int id = Cards::ID_CS2_168;

		Card_CS2_168() {}
	};

	class Card_EX1_508o : public EnchantmentCardBase
	{
	public:
		static constexpr EnchantmentTiers tier = kEnchantmentAura;
		static constexpr int id = Cards::ID_EX1_508o;

		Card_EX1_508o()
		{
			apply_functor = [](auto& stats) {
				++stats.attack;
			};
		}
	};

	class Card_EX1_508 : public MinionCardBase<Card_EX1_508>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_EX1_508;

		Card_EX1_508()
		{
			Aura<Card_EX1_508o>().Target([](auto& context, auto& targetor) {
				targetor
					.Ally(context).Minion().Murlocs() // friendly murlocs only
					.Exclude(context.card_ref_); // only apply on other murlocs
			});
		}
	};

	class Card_CS2_171 : public MinionCardBase<Card_CS2_171>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS2_171;

		Card_CS2_171()
		{
			Charge();
		}
	};

	class Card_EX1_011 : public MinionCardBase<Card_EX1_011>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_EX1_011;

		Card_EX1_011()
		{
			battlecry_target_getter = [](auto context) {
				SetBattlecryTarget(context, Targets().Targetable().Exclude(context.card_ref_));
				return true;
			};
			battlecry = [](auto context) {
				Heal(context).Target(context.flow_context_.battlecry_target_).Amount(2);
			};
		}
	};

	class Card_EX1_066 : public MinionCardBase<Card_EX1_066>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_EX1_066;

		Card_EX1_066()
		{
			battlecry = [](auto context) {
				Manipulate(context).OpponentHero().DestroyWeapon();
			};
		}
	};

	class Card_CS2_172 : public MinionCardBase<Card_CS2_172>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS2_172;
	};

	class Card_CS2_173 : public MinionCardBase<Card_CS2_173>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS2_173;

		Card_CS2_173()
		{
			Charge();
		}
	};

	class Card_CS2_121 : public MinionCardBase<Card_CS2_121>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS2_121;

		Card_CS2_121()
		{
			Taunt();
		}
	};

	class Card_CS2_142 : public MinionCardBase<Card_CS2_142>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS2_142;

		Card_CS2_142()
		{
			SpellDamage(1);
		}
	};

	class Card_EX1_506 : public MinionCardBase<Card_EX1_506>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_EX1_506;

		Card_EX1_506()
		{
			battlecry = [](auto context) {
				Summon(context, Cards::ID_EX1_506a);
			};
		}
	};

	class Card_EX1_506a : public MinionCardBase<Card_EX1_506a>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_EX1_506a;

		Card_EX1_506a()
		{
		}
	};

	class Card_EX1_015 : public MinionCardBase<Card_EX1_015>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_EX1_015;

		Card_EX1_015()
		{
			battlecry = [](auto context) {
				Manipulate(context).CurrentHero().DrawCard();
			};
		}
	};

	class Card_CS2_120 : public MinionCardBase<Card_CS2_120>
	{
	public:
		static constexpr int id = Cards::ID_CS2_120;

		Card_CS2_120() {}
	};

	class Card_EX1_582 : public MinionCardBase<Card_EX1_582>
	{
	public:
		static constexpr int id = Cards::ID_EX1_582;

		Card_EX1_582()
		{
			SpellDamage(1);
		}
	};

	class Card_CS2_141 : public MinionCardBase<Card_CS2_141>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS2_141;

		Card_CS2_141()
		{
			battlecry_target_getter = [](auto context) {
				SetBattlecryTarget(context, Targets().Targetable());
				return true;
			};
			battlecry = [](auto context) {
				Damage(context).Target(context.flow_context_.battlecry_target_).Amount(1);
			};
		}
	};

	class Card_CS2_125 : public MinionCardBase<Card_CS2_125>
	{
	public:
		static constexpr int id = Cards::ID_CS2_125;

		Card_CS2_125()
		{
			Taunt();
		}
	};

	class Card_CS2_118 : public MinionCardBase<Card_CS2_118>
	{
	public:
		static constexpr int id = Cards::ID_CS2_118;

		Card_CS2_118() {}
	};

	class Card_CS2_122e : public EnchantmentCardBase
	{
	public:
		static constexpr EnchantmentTiers tier = kEnchantmentTier1;
		static constexpr int id = Cards::ID_CS2_122e;

		Card_CS2_122e()
		{
			apply_functor = [](auto& stats) {
				++stats.attack;
			};
		}
	};

	class Card_CS2_122 : public MinionCardBase<Card_CS2_122>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS2_122;

		Card_CS2_122()
		{
			Aura<Card_CS2_122e>().Target([](auto& context, auto& targetor) {
				targetor
					.Ally(context).Minion() // friendly minions
					.Exclude(context.card_ref_); // only apply on other murlocs
			});
		}
	};

	class Card_CS2_boar : public MinionCardBase<Card_CS2_boar>
	{
	public:
		static constexpr int id = Cards::ID_CS2_boar;

		Card_CS2_boar() {}
	};

	class Card_CS2_196 : public MinionCardBase<Card_CS2_196>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS2_196;

		Card_CS2_196()
		{
			battlecry = [](auto context) {
				Summon(context, Cards::ID_CS2_boar);
			};
		}
	};

	class Card_EX1_019e : public EnchantmentCardBase
	{
	public:
		static constexpr EnchantmentTiers tier = kEnchantmentTier1;
		static constexpr int id = Cards::ID_EX1_019e;

		Card_EX1_019e()
		{
			apply_functor = [](auto& stats) {
				++stats.max_hp;
				++stats.attack;
			};
		}
	};

	class Card_EX1_019 : public MinionCardBase<Card_EX1_019>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_EX1_019;

		Card_EX1_019()
		{
			battlecry_target_getter = [](auto context) {
				SetBattlecryTarget(context, Targets().Targetable().Ally(context).Minion());
				return true;
			};
			battlecry = [](auto context) {
				Manipulate(context).Minion(context.flow_context_.battlecry_target_)
					.Enchant().Add(Card_EX1_019e());
			};
		}
	};
}

REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_189)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS1_042)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_168)
REGISTER_MINION_CARD_CLASS(Cards::Card_EX1_508)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_171)
REGISTER_MINION_CARD_CLASS(Cards::Card_EX1_011)
REGISTER_MINION_CARD_CLASS(Cards::Card_EX1_066)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_172)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_173)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_121)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_142)
REGISTER_MINION_CARD_CLASS(Cards::Card_EX1_506)
REGISTER_MINION_CARD_CLASS(Cards::Card_EX1_506a)
REGISTER_MINION_CARD_CLASS(Cards::Card_EX1_015)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_120)
REGISTER_MINION_CARD_CLASS(Cards::Card_EX1_582)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_141)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_125)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_118)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_122)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_boar)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_196)
REGISTER_MINION_CARD_CLASS(Cards::Card_EX1_019)
