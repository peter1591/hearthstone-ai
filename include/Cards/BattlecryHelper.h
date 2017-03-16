#pragma once

#include "state/State.h"

namespace Cards
{
	namespace detail {
		template <typename T> class HasBattlecry {
			typedef char One;
			typedef long Two;

			template <typename C> static One test(decltype(&C::Battlecry));
			template <typename C> static Two test(...);

		public:
			enum { value = sizeof(test<T>(0)) == sizeof(One) };
		};
		template <typename T> class HasGetBattlecryTargets {
			typedef char One;
			typedef long Two;

			template <typename C> static One test(decltype(&C::GetBattlecryTargets));
			template <typename C> static Two test(...);

		public:
			enum { value = sizeof(test<T>(0)) == sizeof(One) };
		};

	}

	template <typename T,
		bool has_battlecry = detail::HasBattlecry<T>::value,
		bool has_getter = detail::HasGetBattlecryTargets<T>::value>
		struct BattlecryProcessor;

	template <typename T> struct BattlecryProcessor<T, false, false> {
		BattlecryProcessor(state::Cards::CardData & card_data) {}
	};
	template <typename T> struct BattlecryProcessor<T, true, false> {
		BattlecryProcessor(state::Cards::CardData & card_data) {
			card_data.battlecry_handler.SetCallback_Battlecry((FlowControl::battlecry::Handler::BattlecryCallback*)(&T::Battlecry));
		}
	};
	template <typename T> struct BattlecryProcessor<T, true, true> {
		BattlecryProcessor(state::Cards::CardData & card_data) {
			card_data.battlecry_handler.SetCallback_BattlecryTargetGetter([](auto context) {
				return T::GetBattlecryTargets(std::move(context)).GetInfo();
			});
			card_data.battlecry_handler.SetCallback_Battlecry((FlowControl::battlecry::Handler::BattlecryCallback*)(&T::Battlecry));
		}
	};
}