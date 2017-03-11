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
				SetBattlecryTarget(std::move(context), Targets().Targetable());
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
				SetBattlecryTarget(std::move(context), Targets().Targetable().Exclude(context.card_ref_));
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
				SetBattlecryTarget(std::move(context), Targets().Targetable());
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
					.Exclude(context.card_ref_); // only apply on other
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
				SetBattlecryTarget(std::move(context), Targets().Targetable().Ally(context).Minion());
				return true;
			};
			battlecry = [](auto context) {
				Manipulate(context).Minion(context.flow_context_.battlecry_target_)
					.Enchant().Add(Card_EX1_019e());
			};
		}
	};

	class Card_CS2_127 : public MinionCardBase<Card_CS2_127>
	{
	public:
		static constexpr int id = Cards::ID_CS2_127;

		Card_CS2_127()
		{
			Taunt();
		}
	};

	class Card_CS2_124 : public MinionCardBase<Card_CS2_124>
	{
	public:
		static constexpr int id = Cards::ID_CS2_124;

		Card_CS2_124() { Charge(); }
	};

	class Card_CS2_182 : public MinionCardBase<Card_CS2_182>
	{
	public:
		static constexpr int id = Cards::ID_CS2_182;

		Card_CS2_182() {}
	};

	class Card_EX1_025t : public MinionCardBase<Card_EX1_025t>
	{
	public:
		static constexpr int id = Cards::ID_EX1_025t;

		Card_EX1_025t() {}
	};

	class Card_EX1_025 : public MinionCardBase<Card_EX1_025>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_EX1_025;

		Card_EX1_025()
		{
			battlecry = [](auto context) {
				Summon(context, Cards::ID_EX1_025t);
			};
		}
	};

	class Card_CS2_147 : public MinionCardBase<Card_CS2_147>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS2_147;

		Card_CS2_147()
		{
			battlecry = [](auto context) {
				Manipulate(context).CurrentHero().DrawCard();
			};
		}
	};

	class Card_CS2_119 : public MinionCardBase<Card_CS2_119>
	{
	public:
		static constexpr int id = Cards::ID_CS2_119;

		Card_CS2_119() {}
	};

	class Card_CS2_197 : public MinionCardBase<Card_CS2_197>
	{
	public:
		static constexpr int id = Cards::ID_CS2_197;

		Card_CS2_197() { SpellDamage(1); }
	};

	class Card_CS2_179 : public MinionCardBase<Card_CS2_179>
	{
	public:
		static constexpr int id = Cards::ID_CS2_179;

		Card_CS2_179() { Taunt(); }
	};

	class Card_CS2_131 : public MinionCardBase<Card_CS2_131>
	{
	public:
		static constexpr int id = Cards::ID_CS2_131;

		Card_CS2_131() { Charge(); }
	};

	class Card_CS2_187 : public MinionCardBase<Card_CS2_187>
	{
	public:
		static constexpr int id = Cards::ID_CS2_187;

		Card_CS2_187() { Taunt(); }
	};

	class Card_DS1_055 : public MinionCardBase<Card_DS1_055>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_DS1_055;

		Card_DS1_055()
		{
			battlecry = [](auto context) {
				ForEach(context, Targets().Ally(context).Exclude(context.card_ref_),
					[](state::State & state, state::FlowContext & flow_context, state::CardRef ref) {
					FlowControl::Manipulate(state, flow_context).Character(ref).Heal(2);
				});
			};
		}
	};

	template <int Arg1>
	class Card_CS2_226e : public EnchantmentCardBase
	{
	public:
		static constexpr EnchantmentTiers tier = kEnchantmentTier1;
		static constexpr int id = Cards::ID_EX1_019e;

		Card_CS2_226e()
		{
			apply_functor = [](auto& stats) {
				stats.max_hp += Arg1;
				stats.attack += Arg1;
			};
		}
	};

	class Card_CS2_226 : public MinionCardBase<Card_CS2_226>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS2_226;

		Card_CS2_226()
		{
			battlecry = [](auto context) {
				int count = 0;
				Targets().Ally(context).Minion().Exclude(context.card_ref_).GetInfo()
					.Count(context.state_, &count);

				switch (count) {
				case 0:
					break;
				case 1:
					Manipulate(context).Minion(context.card_ref_).Enchant().Add(Card_CS2_226e<1>());
					break;
				case 2:
					Manipulate(context).Minion(context.card_ref_).Enchant().Add(Card_CS2_226e<2>());
					break;
				case 3:
					Manipulate(context).Minion(context.card_ref_).Enchant().Add(Card_CS2_226e<3>());
					break;
				case 4:
					Manipulate(context).Minion(context.card_ref_).Enchant().Add(Card_CS2_226e<4>());
					break;
				case 5:
					Manipulate(context).Minion(context.card_ref_).Enchant().Add(Card_CS2_226e<5>());
					break;
				case 6:
					Manipulate(context).Minion(context.card_ref_).Enchant().Add(Card_CS2_226e<6>());
					break;
				default:
					throw std::exception("invalid minion count");
				}
			};
		}
	};

	class Card_EX1_399e : public EnchantmentCardBase
	{
	public:
		static constexpr EnchantmentTiers tier = kEnchantmentTier1;
		static constexpr int id = Cards::ID_EX1_399e;

		Card_EX1_399e()
		{
			apply_functor = [](auto& stats) {
				stats.attack += 3;
			};
		}
	};

	class Card_EX1_399 : public MinionCardBase<Card_EX1_399>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_EX1_399;

		Card_EX1_399()
		{
			added_to_play_zone = [](auto context) {
				state::CardRef self = context.card_ref_;
				context.state_.event_mgr.PushBack<state::Events::EventTypes::OnTakeDamage>(
					self,
					[](auto& controller, auto self, auto& context) {
					if (context.damage_ <= 0) return;
					if (!IsAlive(context, self)) return controller.Remove();
					Manipulate(context).Card(self).Enchant().Add(Card_EX1_399e());
				});
			};
		}
	};

	class Card_EX1_593 : public MinionCardBase<Card_EX1_593>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_EX1_593;

		Card_EX1_593()
		{
			battlecry = [](auto context) {
				Damage(context).Opponent().Amount(3);
			};
		}
	};

	class Card_CS2_150 : public MinionCardBase<Card_CS2_150>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS2_150;

		Card_CS2_150()
		{
			battlecry_target_getter = [](auto context) {
				SetBattlecryTarget(std::move(context), Targets().Targetable());
				return true;
			};
			battlecry = [](auto context) {
				Damage(context).Target(context.flow_context_.battlecry_target_).Amount(2);
			};
		}
	};

	class Card_CS2_155 : public MinionCardBase<Card_CS2_155>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS2_155;

		Card_CS2_155()
		{
			SpellDamage(1);
		}
	};

	class Card_CS2_200 : public MinionCardBase<Card_CS2_200>
	{
	public:
		static constexpr int id = Cards::ID_CS2_200;
		Card_CS2_200() {}
	};

	class Card_CS2_162 : public MinionCardBase<Card_CS2_162>
	{
	public:
		static constexpr int id = Cards::ID_CS2_162;
		Card_CS2_162() { Taunt(); }
	};

	class Card_CS2_213 : public MinionCardBase<Card_CS2_213>
	{
	public:
		static constexpr int id = Cards::ID_CS2_213;
		Card_CS2_213() { Charge(); }
	};

	class Card_CS2_201 : public MinionCardBase<Card_CS2_201>
	{
	public:
		static constexpr int id = Cards::ID_CS2_201;
		Card_CS2_201() {}
	};

	class Card_CS2_222o : public EnchantmentCardBase
	{
	public:
		static constexpr EnchantmentTiers tier = kEnchantmentAura;
		static constexpr int id = Cards::ID_CS2_222o;

		Card_CS2_222o()
		{
			apply_functor = [](auto& stats) {
				++stats.attack;
				++stats.max_hp;
			};
		}
	};

	class Card_CS2_222 : public MinionCardBase<Card_CS2_222>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS2_222;

		Card_CS2_222()
		{
			Aura<Card_CS2_222o>().Target([](auto& context, auto& targetor) {
				targetor
					.Ally(context).Minion() // friendly minions
					.Exclude(context.card_ref_); // only apply on other
			});
		}
	};

	class Card_CS2_186 : public MinionCardBase<Card_CS2_186>
	{
	public:
		static constexpr int id = Cards::ID_CS2_186;

		Card_CS2_186() {}
	};
}

REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_186)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_222)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_201)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_213)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_162)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_200)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_155)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_150)
REGISTER_MINION_CARD_CLASS(Cards::Card_EX1_593)
REGISTER_MINION_CARD_CLASS(Cards::Card_EX1_399)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_226)
REGISTER_MINION_CARD_CLASS(Cards::Card_DS1_055)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_187)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_131)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_179)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_197)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_119)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_147)
REGISTER_MINION_CARD_CLASS(Cards::Card_EX1_025t)
REGISTER_MINION_CARD_CLASS(Cards::Card_EX1_025)

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
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_127)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_124)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_182)
