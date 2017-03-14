#pragma once

#include "state/State.h"

namespace Cards
{
	// event lifetime
	struct InPlayZone {
		template <typename Functor>
		static void RegisterEvent(state::Cards::CardData& card_data, Functor&& functor) {
			card_data.added_to_play_zone += std::move(functor);
		}
		static bool StillValid(state::Cards::Card const& card) {
			return card.GetZone() == state::kCardZonePlay;
		}
	};
	struct InHandZone {};
	struct InDeckZone {};

	struct NonCategorized_SelfInLambdaCapture {}; // remember self on lambda capture
	struct CateogrizedOnSelf {}; // remember self on categorized event

	// helper
	namespace detail {
		template <typename EventHandler, typename EventHandlerArg> struct EventHandlerInvoker {
			template <typename... Args> static auto Invoke(Args&&... args) {
				return EventHandler::Invoke<EventHandlerArg>(std::forward<Args>(args)...);
			}
		};
		template <typename EventHandler> struct EventHandlerInvoker<EventHandler, void> {
			template <typename... Args> static auto Invoke(Args&&... args) {
				return EventHandler::Invoke(std::forward<Args>(args)...);
			}
		};

		template <typename LifeTime, typename SelfPolicy, typename EventType, typename EventHandler, typename EventHandlerArg> struct AddEventHelper;

		template <typename LifeTime, typename EventType, typename EventHandler, typename EventHandlerArg>
		struct AddEventHelper<LifeTime, NonCategorized_SelfInLambdaCapture, EventType, EventHandler, EventHandlerArg> {
			template <typename Context>
			static void AddEvent(state::CardRef self, Context&& context) {
				context.state_.AddEvent<EventType>(
					[self](auto& controller, auto& context) {
					if (!LifeTime::StillValid(context.state_.GetCard(self))) return controller.Remove();
					EventHandlerInvoker<EventHandler, EventHandlerArg>::Invoke(std::move(controller), self, std::move(context));
				});
			}
		};
		template <typename LifeTime, typename EventType, typename EventHandler, typename EventHandlerArg>
		struct AddEventHelper<LifeTime, CateogrizedOnSelf, EventType, EventHandler, EventHandlerArg> {
			template <typename Context>
			static void AddEvent(state::CardRef self, Context&& context) {
				context.state_.AddEvent<EventType>(
					self,
					[](auto& controller, state::CardRef self, auto& context) {
					if (!LifeTime::StillValid(context.state_.GetCard(self))) return controller.Remove();
					EventHandlerInvoker<EventHandler, EventHandlerArg>::Invoke(std::move(controller), self, std::move(context));
				});
			}
		};
	}

	template <typename LifeTime, typename SelfPolicy, typename EventType, typename EventHandler, typename EventHandlerArg = void>
	struct EventRegisterHelper {
		EventRegisterHelper(state::Cards::CardData & card_data) {
			LifeTime::RegisterEvent(card_data, [](auto context) {
				state::CardRef self = context.card_ref_;
				detail::AddEventHelper<LifeTime, SelfPolicy, EventType, EventHandler, EventHandlerArg>
					::AddEvent(self, std::move(context));
			});
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

	// helper for common usages
	struct OnSelfTakeDamage {
		using LifeTime = InPlayZone;
		using SelfPolicy = CateogrizedOnSelf;
		using EventType = state::Events::EventTypes::OnTakeDamage;

		struct EventHandler {
			template <typename UnderlyingHandler, typename Controller, typename Context>
			static void Invoke(Controller&& controller, state::CardRef self, Context&& context) {
				if (context.damage_ <= 0) return;
				return UnderlyingHandler::Invoke(std::move(controller), self, std::move(context));
			}
		};
	};
}