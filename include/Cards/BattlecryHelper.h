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
		template <typename T> class HasGetSpecifiedTargets {
			typedef char One;
			typedef long Two;

			template <typename C> static One test(decltype(&C::GetSpecifiedTargets));
			template <typename C> static Two test(...);

		public:
			enum { value = sizeof(test<T>(0)) == sizeof(One) };
		};

	}

	template <typename T,
		bool has_battlecry = detail::HasBattlecry<T>::value,
		bool has_getter = detail::HasGetSpecifiedTargets<T>::value>
		struct BattlecryProcessor;

	template <typename T> struct BattlecryProcessor<T, false, false> {
		BattlecryProcessor(state::Cards::CardData & card_data) {}
	};
	template <typename T> struct BattlecryProcessor<T, true, false> {
		BattlecryProcessor(state::Cards::CardData & card_data) {
			card_data.onplay_handler.SetOnPlayCallback((FlowControl::onplay::Handler::OnPlayCallback*)(&T::Battlecry));
		}
	};
	template <typename T> struct BattlecryProcessor<T, true, true> {
		BattlecryProcessor(state::Cards::CardData & card_data) {
			card_data.onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.targets_ = T::GetSpecifiedTargets(std::move(context)).GetInfo();
				return true;
			});
			card_data.onplay_handler.SetOnPlayCallback((FlowControl::onplay::Handler::OnPlayCallback*)(&T::Battlecry));
		}
	};
}