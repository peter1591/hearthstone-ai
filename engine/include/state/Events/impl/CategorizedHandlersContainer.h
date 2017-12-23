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
				CategorizedHandlersContainer() : base_(nullptr), handlers_() {}

				CategorizedHandlersContainer(CategorizedHandlersContainer const& rhs) :
					base_(rhs.base_),
					handlers_(rhs.handlers_)
				{}

				CategorizedHandlersContainer & operator=(CategorizedHandlersContainer const& rhs) {
					base_ = rhs.base_;
					handlers_ = rhs.handlers_;
					return *this;
				}

				void RefCopy(CategorizedHandlersContainer<TriggerType> const& base) {
					assert(base.base_ == nullptr);
					base_ = &base.handlers_;
				}

				// Cloneable by copy semantics
				//    Since the STL container and HandlersContainer are with this property
				static const bool CloneableByCopySemantics = true;

				template <typename HandlerType_>
				void PushBack(CardRef card_ref, HandlerType_&& handler)
				{
					static_assert(std::is_convertible_v<std::decay_t<HandlerType_>, typename TriggerType::type>, "Wrong type");
					GetContainerForWrite().push_back(
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

					if (GetContainerForRead().empty()) return;

					size_t origin_size = GetContainerForRead().size();
					for (size_t idx = 0; idx < GetContainerForRead().size();) {
						if (idx >= origin_size) break; // do not trigger newly-added handlers

						auto const& item = GetContainerForRead()[idx];
						if (item.card_ref != card_ref) {
							++idx;
							continue;
						}

						auto ret = (item.handler)(card_ref, std::forward<Args>(args)...);
						static_assert(std::is_same_v<decltype(ret), bool>, "Should return a boolean flag indicating if we should remove the item.");

						if (!ret) {
							GetContainerForWrite().erase(GetContainerForWrite().begin() + idx);
						}
						else {
							++idx;
						}
					}
				}

				template <typename... Args>
				void TriggerAllWithoutRemove(CardRef card_ref, Args&&... args) const
				{
					// We store all the triggers to a temporary queue, and this queue is immutable during resolve.
					// Reference: http://hearthstone.gamepedia.com/Advanced_rulebook
					//   "Rule 2c: A Queue becomes immutable once Hearthstone starts to resolve the first entry in it. No new entries can be added to the Queue after this point."

					// Example:
					//    If you play a spell, Troggzor the Earthinator summons a Burly Rockjaw Trogg.
					//    The Burly Rockjaw Trogg does not trigger from the same spell because the consequences of playing the spell have already begun resolving.

					if (GetContainerForRead().empty()) return;

					size_t origin_size = GetContainerForRead().size();
					for (size_t idx = 0; idx < GetContainerForRead().size(); ++idx) {
						if (idx >= origin_size) break; // do not trigger newly-added handlers

						auto const& item = GetContainerForRead()[idx];
						if (item.card_ref != card_ref) {
							continue;
						}

						(item.handler)(card_ref, std::forward<Args>(args)...);
					}
				}

			private:
				using container_type = std::vector<Item>;

				container_type const& GetContainerForRead() {
					if (base_) return *base_;
					return handlers_;
				}

				container_type & GetContainerForWrite() {
					if (base_) {
						handlers_ = *base_; // copy-on-write
						base_ = nullptr;
					}
					return handlers_;
				}

			private:
				container_type const* base_;
				container_type handlers_;
			};
		}
	}
}