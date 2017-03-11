#pragma once

#include <vector>
#include <type_traits>
#include <utility>

#include "state/Events/HandlersContainerController.h"

namespace state
{
	namespace Events
	{
		namespace impl {
			template <typename TriggerType>
			class HandlersContainer
			{
			public:
				// Cloneable by copy semantics
				//    Since the STL container and the underlying HandlerType are with this property
				static const bool CloneableByCopySemantics = true;

				template <typename T>
				void PushBack(T&& handler)
				{
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

					// Since we don't have insert method to outsiders, we can just write down the last iterator to specify the temporary queue
					auto it_last = handlers_.end();
					--it_last;

					while (true)
					{
						const bool exiting = (it == it_last);
						HandlersContainerController controller;

						(*it)(controller, std::forward<Args>(args)...);

						if (controller.IsRemoved()) { it = handlers_.erase(it); }
						else { ++it; }

						if (exiting) break;
					}
				}

			private:
				typedef std::vector<typename TriggerType::FunctorType> container_type;
				container_type handlers_;
			};
		}
	}
}