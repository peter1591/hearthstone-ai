#pragma once

#include <functional>
#include <type_traits>
#include <utility>
#include "Utils/CloneableContainers/Vector.h"

// Use a boolean flag to mark if an item has been removed or not

namespace Utils
{
	namespace CloneableContainers
	{
		template <typename ItemType> class RemovableVector;
		class RemovableVectorIdentifierHasher;

		class RemovableVectorIdentifier
		{
			template <typename ItemType> friend class RemovableVector;
			friend class RemovableVectorIdentifierHasher;

			typedef typename CloneableContainers::VectorIdentifier impl_identifier_type;

		public:
			RemovableVectorIdentifier() {}
			RemovableVectorIdentifier(const RemovableVectorIdentifier & rhs) = default;
			RemovableVectorIdentifier & operator=(const RemovableVectorIdentifier & rhs) = default;
			RemovableVectorIdentifier(RemovableVectorIdentifier && rhs) = default;
			RemovableVectorIdentifier & operator=(RemovableVectorIdentifier && rhs) = default;

			bool operator==(const RemovableVectorIdentifier& rhs) const { return identifier_ == rhs.identifier_; }
			bool operator!=(const RemovableVectorIdentifier& rhs) const { return identifier_ != rhs.identifier_; }

		private:
			RemovableVectorIdentifier(const impl_identifier_type & identifier) : identifier_(identifier) {}
			RemovableVectorIdentifier(impl_identifier_type && identifier) : identifier_(std::move(identifier)) {}

			impl_identifier_type identifier_;
		};

		class RemovableVectorIdentifierHasher
		{
		public:
			size_t operator()(const RemovableVectorIdentifier & id) const { return std::hash<decltype(id.identifier_)>()(id.identifier_); }
		};

		template <typename ItemType>
			class RemovableVector
		{
		private:
			struct InternalItemType
			{
				InternalItemType(InternalItemType const& rhs) = default;
				InternalItemType(InternalItemType && rhs) = default;
				InternalItemType & operator=(InternalItemType const& rhs) = default;
				InternalItemType & operator=(InternalItemType && rhs) = default;

				template <typename T>
				explicit InternalItemType(T&& item, VectorIdentifier next_possible_exist_id) :
					removed(false), item(std::forward<T>(item)),
					next_possible_exist_id(next_possible_exist_id)
				{
				}

				bool removed;
				ItemType item;
				VectorIdentifier next_possible_exist_id;
			};

		public:
			typedef RemovableVectorIdentifier Identifier;
			typedef RemovableVectorIdentifierHasher IdentifierHasher;

		public:
			RemovableVector() :
				items_(),
				first_possible_exist_id_(items_.GetNextPushBackItemIdentifier())
			{
			}

			RemovableVector(size_t default_capacity) :
				items_(default_capacity),
				first_possible_exist_id_(items_.GetNextPushBackItemIdentifier())
			{
			}

			template <typename T>
			Identifier PushBack(T&& item) {
				return items_.PushBack(InternalItemType(std::forward<T>(item), items_.GetNextNextPushBackItemIdentifier()));
			}

			const ItemType * Get(Identifier identifier) const {
				if (identifier.identifier_ < first_possible_exist_id_) return nullptr;
				auto & item = items_.Get(identifier.identifier_);
				if (item.removed) return nullptr;
				else return &item.item;
			}

			ItemType * Get(Identifier identifier) {
				if (identifier.identifier_ < first_possible_exist_id_) return nullptr;
				auto & item = items_.Get(identifier.identifier_);
				if (item.removed) return nullptr;
				else return &item.item;
			}

			void Remove(Identifier identifier) {
				items_.Get(identifier.identifier_).removed = true;
			}

			void Clear() {
				// Note: the 'removed' flag might not be set
				first_possible_exist_id_ = items_.GetEnd();
			}

			void Reset() {
				// All iterators are invalidated
				items_.Reset();
				first_possible_exist_id_ = items_.GetNextPushBackItemIdentifier();
			}

		public: // iterate
			// Only iterate through exist items
			template <typename IterateCallback> // bool(ItemType&), return true to continue; false to abort
			void IterateAll(IterateCallback&& callback)
			{
				VectorIdentifier id = first_possible_exist_id_;
				VectorIdentifier id_end = items_.GetEnd();

				if (id == id_end) return; // fast path

				VectorIdentifier * prev_hint = &first_possible_exist_id_;
				bool need_update = false; // need update prev_hint?

				while (id != id_end) {
					InternalItemType & item = items_.Get(id);

					if (item.removed) {
						need_update = true;
						id = item.next_possible_exist_id;
						continue;
					}

					if (need_update) *prev_hint = id;

					prev_hint = &item.next_possible_exist_id;
					need_update = false;

					if (!callback(id, item.item)) break;
					id = item.next_possible_exist_id;
				}
			}

		private:
			CloneableContainers::Vector<InternalItemType> items_;
			VectorIdentifier first_possible_exist_id_;
		};
	}
}