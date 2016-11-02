#pragma once

#include <utility>
#include "CloneableContainers/Vector.h"

// Use a boolean flag to mark if an item has been removed or not

namespace CloneableContainers
{
	template <typename ItemType>
	class RemovableVector
	{
	private:
		struct InternalItemType
		{
			explicit InternalItemType(ItemType && item) : removed(false), item(std::forward<ItemType>(item)) {}

			bool removed;
			ItemType item;
		};

	public:
		class Identifier
		{
			friend class RemovableVector<ItemType>;

			typedef typename CloneableContainers::Vector<InternalItemType>::Identifier impl_identifier_type;

			Identifier(impl_identifier_type && identifier) : identifier_(std::forward<impl_identifier_type>(identifier)) {}
			impl_identifier_type identifier_;
		};

	public:
		RemovableVector() {}
		RemovableVector(size_t defualt_capacity) : items_(default_capacity) {}

		template <typename... Args>
		Identifier PushBack(Args&&... args) {
			return items_.PushBack(InternalItemType(ItemType(std::forward<ItemType>(args)...)));
		}

		const ItemType * Get(Identifier identifier) const {
			auto & item = items_.Get(identifier);
			if (item.removed) return nullptr;
			else return &item.item;
		}

		ItemType * Get(Identifier identifier) {
			auto & item = items_.Get(identifier.identifier_);
			if (item.removed) return nullptr;
			else return &item.item;
		}

		void Remove(Identifier identifier) {
			items_.Get(identifier.identifier_).removed = true;
		}

	public: // iterate
		Identifier GetFirst()
		{
			auto id = Identifier(items_.GetFirst());
			StepToNextExistItem(id);
			return id;
		}

		void StepNext(Identifier & id)
		{
			items_.StepNext(id.identifier_);
			StepToNextExistItem(id);
		}

		bool ReachedEnd(const Identifier &id)
		{
			return items_.ReachedEnd(id.identifier_);
		}

	private:
		void StepToNextExistItem(Identifier & id)
		{
			while (!items_.ReachedEnd(id.identifier_)) {
				if (!items_.Get(id.identifier_).removed) return;
				StepNext(id);
			}
		}

	private:
		CloneableContainers::Vector<InternalItemType> items_;
	};
}