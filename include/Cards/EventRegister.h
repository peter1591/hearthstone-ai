#pragma once

#include "state/State.h"

namespace Cards
{
	// event lifetime
	struct InPlayZone {};
	struct InHandZone {};
	struct InDeckZone {};

	// helper
	namespace detail {
		struct EventNonCateogrizedEvent {};
		struct EventCategorizedEventOnSelf {};
		template <typename LifeTime, typename EventCategoryType, typename EventType, typename EventHandler, typename EventHandlerArg> struct EventRegisterHelper;

		template <typename EventType, typename EventHandler, typename EventHandlerArg>
		struct EventRegisterHelper<InPlayZone, EventNonCateogrizedEvent, EventType, EventHandler, EventHandlerArg> {
			EventRegisterHelper(state::Cards::CardData & card_data) {
				card_data.added_to_play_zone += [](auto context) {
					context.state_.AddEvent<EventType>(
						[](auto& controller, state::CardRef card_ref, auto& context) {
						if (context.card_.GetZone() != state::kCardZonePlay) {
							controller.Remove();
							return;
						}
						EventHandler::Invoke<EventHandlerArg>(std::move(controller), card_ref, std::move(context));
					});
				};
			}
		};
		template <typename EventType, typename EventHandler, typename EventHandlerArg>
		struct EventRegisterHelper<InPlayZone, EventCategorizedEventOnSelf, EventType, EventHandler, EventHandlerArg> {
			EventRegisterHelper(state::Cards::CardData & card_data) {
				card_data.added_to_play_zone += [](auto context) {
					state::CardRef self = context.card_ref_;
					context.state_.AddEvent<EventType>(
						self,
						[](auto& controller, auto self, auto& context) {
						if (context.card_.GetZone() != state::kCardZonePlay) {
							controller.Remove();
							return;
						}
						EventHandler::Invoke<EventHandlerArg>(std::move(controller), self, std::move(context));
					});
				};
			}
		};
	}

	// event types
	struct OnSelfTakeDamage {
		typedef InPlayZone LifeTime;
		typedef detail::EventCategorizedEventOnSelf EventCategoryType;
		typedef state::Events::EventTypes::OnTakeDamage EventType;

		struct EventHandler {
			template <typename UnderlyingHandler, typename Controller, typename Context>
			static void Invoke(Controller&& controller, state::CardRef self, Context&& context) {
				if (context.damage_ <= 0) return;
				return UnderlyingHandler::Invoke(std::move(controller), self, std::move(context));
			}
		};
	};

	// event register
	template <typename EventType, typename EventHandler> class EventRegister
		: detail::EventRegisterHelper<
		typename EventType::LifeTime,
		typename EventType::EventCategoryType,
		typename EventType::EventType,
		typename EventType::EventHandler,
		EventHandler>
	{
	public:
		EventRegister(state::Cards::CardData& card_data) : EventRegisterHelper(card_data) {}
	};
}