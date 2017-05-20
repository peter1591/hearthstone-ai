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
					case 0: return Add<T<0>>(); // TODO: replace followings
					case 1: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<1>());
					case 2: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<2>());
					case 3: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<3>());
					case 4: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<4>());
					case 5: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<5>());
					case 6: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<6>());
					case 7: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<7>());
					case 8: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<8>());
					case 9: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<9>());
					case 10: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<10>());
					case 11: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<11>());
					case 12: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<12>());
					case 13: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<13>());
					case 14: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<14>());
					case 15: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<15>());
					case 16: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<16>());
					case 17: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<17>());
					case 18: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<18>());
					case 19: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<19>());
					case 20: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<20>());
					case 21: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<21>());
					case 22: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<22>());
					case 23: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<23>());
					case 24: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<24>());
					case 25: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<25>());
					case 26: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<26>());
					case 27: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<27>());
					case 28: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<28>());
					case 29: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<29>());
					case 30: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<30>());
					case 31: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<31>());
					case 32: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<32>());
					case 33: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<33>());
					case 34: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<34>());
					case 35: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<35>());
					case 36: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<36>());
					case 37: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<37>());
					case 38: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<38>());
					case 39: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<39>());
					case 40: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<40>());
					case 41: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<41>());
					case 42: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<42>());
					case 43: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<43>());
					case 44: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<44>());
					case 45: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<45>());
					case 46: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<46>());
					case 47: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<47>());
					case 48: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<48>());
					case 49: return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<49>());
					}
					assert(false); // need more switch-case
					return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().PushBackNormalEnchantment(state_, T<50>());
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

				void Update(bool allow_hp_reduce = false) {
					return state_.GetMutableCard(card_ref_).GetMutableEnchantmentHandler().Update(state_, flow_context_, card_ref_, allow_hp_reduce);
				}

			private:
				state::State & state_;
				FlowControl::FlowContext & flow_context_;
				state::CardRef card_ref_;
			};
		}
	}
}