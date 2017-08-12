#pragma once

#include <utility>
#include <unordered_map>
#include "state/Events/impl/HandlersContainer.h"
#include "state/Types.h"

namespace state
{
	namespace Events
	{
		namespace impl
		{
			template <typename TriggerType>
			class CategorizedHandlersContainer
			{
			private:
				struct Item {
					state::CardRef card_ref;
					typename TriggerType::type handler;
				};

			public:
				CategorizedHandlersContainer() : handlers_() {}

				// Cloneable by copy semantics
				//    Since the STL container and HandlersContainer are with this property
				static const bool CloneableByCopySemantics = true;

				template <typename HandlerType_>
				void PushBack(CardRef card_ref, HandlerType_&& handler)
				{
					static_assert(std::is_convertible_v<std::decay_t<HandlerType_>, typename TriggerType::type>, "Wrong type");
					handlers_.push_back(
						Item{ card_ref, std::forward<HandlerType_>(handler) });
				}

				template <typename... Args>
				void TriggerAll(CardRef card_ref, Args&&... args)
				{
					// We store all the triggers to a temporary queue, and this queue is immutable during resolve.
					// Reference: http://hearthstone.gamepedia.com/Advanced_rulebook
					//   "Rule 2c: A Queue becomes immutable once Hearthstone starts to resolve the first entry in it. No new entries can be added to the Queue after this point."

					// Example:
					//    If you play a spell, Troggzor the Earthinator summons a Burly Rockjaw Trogg.
					//    The Burly Rockjaw Trogg does not trigger from the same spell because the consequences of playing the spell have already begun resolving.

					if (handlers_.empty()) return;

					auto it = handlers_.begin();
					size_t rest = handlers_.size();

					while (rest > 0)
					{
						if (it->card_ref != card_ref) {
							++it;
							--rest;
							continue;
						}

						auto ret = (it->handler)(card_ref, std::forward<Args>(args)...);
						static_assert(std::is_same_v<decltype(ret), bool>, "Should return a boolean flag indicating if we should remove the item.");
						if (!ret) it = handlers_.erase(it);
						else ++it;
						--rest;
					}
				}

			private:
				std::vector<Item> handlers_;
			};
		}
	}
}