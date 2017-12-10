#pragma once

#include <assert.h>
#include <memory>
#include <utility>
#include "state/Cards/Card.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			class EnchantmentHelper
			{
			public:
				EnchantmentHelper(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref) :
					state_(state), flow_context_(flow_context), card_ref_(card_ref)
				{
				}

				template <typename EnchantmentType> auto Add(EnchantmentType&& enchantment) {
					return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, std::forward<EnchantmentType>(enchantment));
				}
				template <typename EnchantmentType> auto Add() {
					return Add(EnchantmentType());
				}
				template <template <int> typename T> auto Add(int v) {
					assert(v >= 0);
					switch (v) {
					case 0: return Add<T<0>>();
					case 1: return Add<T<1>>();
					case 2: return Add<T<2>>();
					case 3: return Add<T<3>>();
					case 4: return Add<T<4>>();
					case 5: return Add<T<5>>();
					case 6: return Add<T<6>>();
					case 7: return Add<T<7>>();
					case 8: return Add<T<8>>();
					case 9: return Add<T<9>>();
					case 10: return Add<T<10>>();
					case 11: return Add<T<11>>();
					case 12: return Add<T<12>>();
					case 13: return Add<T<13>>();
					case 14: return Add<T<14>>();
					case 15: return Add<T<15>>();
					case 16: return Add<T<16>>();
					case 17: return Add<T<17>>();
					case 18: return Add<T<18>>();
					case 19: return Add<T<19>>();
					case 20: return Add<T<20>>();
					case 21: return Add<T<21>>();
					case 22: return Add<T<22>>();
					case 23: return Add<T<23>>();
					case 24: return Add<T<24>>();
					case 25: return Add<T<25>>();
					case 26: return Add<T<26>>();
					case 27: return Add<T<27>>();
					case 28: return Add<T<28>>();
					case 29: return Add<T<29>>();
					case 30: return Add<T<30>>();
					case 31: return Add<T<31>>();
					case 32: return Add<T<32>>();
					case 33: return Add<T<33>>();
					case 34: return Add<T<34>>();
					case 35: return Add<T<35>>();
					case 36: return Add<T<36>>();
					case 37: return Add<T<37>>();
					case 38: return Add<T<38>>();
					case 39: return Add<T<39>>();
					case 40: return Add<T<40>>();
					case 41: return Add<T<41>>();
					case 42: return Add<T<42>>();
					case 43: return Add<T<43>>();
					case 44: return Add<T<44>>();
					case 45: return Add<T<45>>();
					case 46: return Add<T<46>>();
					case 47: return Add<T<47>>();
					case 48: return Add<T<48>>();
					case 49: return Add<T<49>>();
					}
					assert(false); // need more switch-case
					return Add<T<50>>();
				}
				template <template <state::PlayerSide> typename T> auto Add(state::PlayerIdentifier player) {
					if (player.IsFirst()) {
						return Add<T<state::kPlayerFirst>>();
					}
					else {
						assert(player.IsSecond());
						return Add<T<state::kPlayerSecond>>();
					}
				}

				template <typename EnchantmentType>
				auto AddEventHooked(EnchantmentType&& enchant) {
					return AddEventHooked(std::forward<EnchantmentType>(enchant), enchantment::Enchantments::EventHookedEnchantment::AuxData());
				}
				template <typename EnchantmentType>
				auto AddEventHooked(EnchantmentType&& enchant, enchantment::Enchantments::EventHookedEnchantment::AuxData const& aux_data) {
					return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackEventHookedEnchantment(
						FlowControl::Manipulate(state_, flow_context_), card_ref_, std::forward<EnchantmentType>(enchant), aux_data);
				}

				void SetHealthToMaxHP();

				template <typename T> auto AddAuraEnchantment(T&& enchantment) {
					return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackAuraEnchantment(state_, std::forward<T>(enchantment));
				}

				template <typename T> auto Exists(T&& id) {
					return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().Exists(std::forward<T>(id));
				}

				template <typename T> auto Remove(T&& id) {
					return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().Remove(std::forward<T>(id));
				}

				void Update(bool allow_hp_reduce = false);

			private:
				state::State & state_;
				FlowControl::FlowContext & flow_context_;
				state::CardRef card_ref_;
			};
		}
	}
}