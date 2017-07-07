#pragma once

#include <vector>
#include <type_traits>
#include <utility>

namespace state
{
	namespace Events
	{
		namespace impl {
			template <typename TriggerType>
			class HandlersContainer
			{
			public:
				HandlersContainer() : handlers_() {}

				// Cloneable by copy semantics
				//    Since the STL container and the underlying HandlerType are with this property
				static const bool CloneableByCopySemantics = true;

				template <typename T>
				void PushBack(T&& handler)
				{
					static_assert(std::is_convertible_v<std::decay_t<T>, typename TriggerType::type>, "Wrong type");
					handlers_.push_back(std::forward<T>(handler));
				}

				// Note: Do not provide remove interface to outside
				// Note: Do not provide insert interface to outside
				//    Or, we need a new algo for TriggerAll()

				template <typename... Args>
				void TriggerAll(Args&&... args)
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
						auto ret = (*it)(std::forward<Args>(args)...);
						static_assert(std::is_same_v<decltype(ret), bool>, "Should return a boolean flag indicating if we should remove the item.");

						if (!ret) it = handlers_.erase(it);
						else ++it;

						--rest;
					}
				}

			private:
				typedef std::vector<typename TriggerType::type> container_type;
				container_type handlers_;
			};
		}
	}
}