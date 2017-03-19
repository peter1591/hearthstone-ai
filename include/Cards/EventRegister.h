#pragma once

#include "Utils/StaticInvokables.h"
#include "state/State.h"

namespace Cards
{
	// event lifetime
	struct InPlayZone {
		template <typename YesFunctor, typename NoFunctor> using AddedToPlayZone = YesFunctor;
		template <typename YesFunctor, typename NoFunctor> using AddedToHandZone = NoFunctor;

		static bool StillValid(state::Cards::Card const& card) {
			return card.GetZone() == state::kCardZonePlay;
		}
	};
	struct InHandZone {};
	struct InDeckZone {};

	// self policy
	// Note: we need to remember 'self' to check if owner is still alive (depends on lifetime policy)
	struct NonCategorized_SelfInLambdaCapture {}; // remember self on lambda capture
	struct CateogrizedOnSelf {}; // remember self on categorized event

	// helper
	namespace detail {
		template <typename EventHandler, typename EventHandlerArg> struct EventHandlerInvoker {
			template <typename... Args> static auto Invoke(Args&&... args) {
				return EventHandler::HandleEvent<EventHandlerArg>(std::forward<Args>(args)...);
			}
		};
		template <typename EventHandler> struct EventHandlerInvoker<EventHandler, void> {
			template <typename... Args> static auto Invoke(Args&&... args) {
				return EventHandler::HandleEvent(std::forward<Args>(args)...);
			}
		};

		template <typename LifeTime, typename SelfPolicy, typename EventType, typename EventHandler, typename EventHandlerArg> struct AddEventHelper;

		template <typename LifeTime, typename EventType, typename EventHandler, typename EventHandlerArg>
		struct AddEventHelper<LifeTime, NonCategorized_SelfInLambdaCapture, EventType, EventHandler, EventHandlerArg> {
			template <typename Context>
			static void AddEvent(state::CardRef self, Context&& context) {
				context.state_.AddEvent<EventType>(
					[self](auto&& context) {
					if (!LifeTime::StillValid(context.state_.GetCard(self))) return false;
					return EventHandlerInvoker<EventHandler, EventHandlerArg>::Invoke(self, std::move(context));
				});
			}
		};
		template <typename LifeTime, typename EventType, typename EventHandler, typename EventHandlerArg>
		struct AddEventHelper<LifeTime, CateogrizedOnSelf, EventType, EventHandler, EventHandlerArg> {
			template <typename Context>
			static void AddEvent(state::CardRef self, Context&& context) {
				context.state_.AddEvent<EventType>(
					self,
					[](state::CardRef self, auto context) {
					if (!LifeTime::StillValid(context.state_.GetCard(self))) return false;
					return EventHandlerInvoker<EventHandler, EventHandlerArg>::Invoke(self, std::move(context));
				});
			}
		};
	}

	template <typename LifeTime, typename SelfPolicy, typename EventType, typename EventHandler, typename EventHandlerArg = void>
	struct NullEventRegisterHelper {
		struct AddedToPlayZoneInvoker {
			template <typename Context>
			static void Invoke(Context context) {
				return;
			}
		};
		using AddedToPlayZone = Utils::StaticInvokableChain<AddedToPlayZoneInvoker>;
	};
	template <typename LifeTime, typename SelfPolicy, typename EventType, typename EventHandler, typename EventHandlerArg = void>
	struct OneEventRegisterHelper {
		struct Invoker {
			template <typename Context>
			static void Invoke(Context context) {
				state::CardRef self = context.card_ref_;
				detail::AddEventHelper<LifeTime, SelfPolicy, EventType, EventHandler, EventHandlerArg>
					::AddEvent(self, std::move(context));
			}
		};
		struct NullInvoker {
			template <typename Context> static void Invoke(Context context) {}
		};

		struct AddedToPlayZoneInvoker {
			template <typename Context>
			static void Invoke(Context context) {
				return LifeTime::AddedToPlayZone<Invoker, NullInvoker>::Invoke(std::move(context));
			}
		};
		using AddedToPlayZone = Utils::StaticInvokableChain<AddedToPlayZoneInvoker>;
	};

	template <typename FirstEvent, typename SecondEvent>
	struct CombineEvents {
		struct AddedToPlayZoneInvoker {
			template <typename Context>
			static void Invoke(Context context) {
				FirstEvent::AddedToPlayZone::Invoke(std::move(context));
				SecondEvent::AddedToPlayZone::Invoke(std::move(context));
			}
		};
		using AddedToPlayZone = Utils::StaticInvokableChain<AddedToPlayZoneInvoker>;
	};

	template <typename Event1, typename Event2>
	struct EventsRegisterHelper {
		EventsRegisterHelper(state::Cards::CardData & card_data) {
			using CombinedEvent = CombineEvents<Event1, Event2>;
			card_data.added_to_play_zone += (state::Cards::CardData::AddedToPlayZoneCallback*)
				(CombinedEvent::AddedToPlayZone::Invoke);
		}
	};

	template <typename LifeTime, typename SelfPolicy, typename EventType, typename EventHandler, typename EventHandlerArg = void>
	struct EventRegisterHelper {
		EventRegisterHelper(state::Cards::CardData & card_data) {
			using FirstEvent = OneEventRegisterHelper<LifeTime, SelfPolicy, EventType, EventHandler, EventHandlerArg>;
			using SecondEvent = NullEventRegisterHelper<LifeTime, SelfPolicy, EventType, EventHandler, EventHandlerArg>;
			using CombinedEvent = CombineEvents<FirstEvent, SecondEvent>;

			card_data.added_to_play_zone += (state::Cards::CardData::AddedToPlayZoneCallback*)
				(CombinedEvent::AddedToPlayZone::Invoke);
		}
	};

	// event register: use helper for common usages
	template <typename EventType, typename EventHandler = void>
	using EventRegister = EventRegisterHelper<
		typename EventType::LifeTime,
		typename EventType::SelfPolicy,
		typename EventType::EventType,
		typename EventType::EventHandler,
		EventHandler>;

	// helpers for common usages
	struct OnSelfTakeDamage {
		using LifeTime = InPlayZone;
		using SelfPolicy = CateogrizedOnSelf;
		using EventType = state::Events::EventTypes::OnTakeDamage;

		struct EventHandler {
			template <typename UnderlyingHandler, typename Context>
			static bool HandleEvent(state::CardRef self, Context&& context) {
				if (context.damage_ <= 0) return true;
				return UnderlyingHandler::HandleEvent(self, std::move(context));
			}
		};
	};
}