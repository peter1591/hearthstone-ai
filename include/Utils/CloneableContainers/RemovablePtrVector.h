#pragma once

#include <utility>
#include <memory>
#include <type_traits>
#include "Utils/CloneableContainers/RemovableVector.h"
#include "Utils/CopyByCloneWrapper.h"

namespace Utils
{
	namespace CloneableContainers
	{
		template <typename PtrItemType,
			typename std::enable_if<std::is_pointer<PtrItemType>::value, int>::type = 0>
			class RemovablePtrVector
		{
		private:
			typedef typename std::remove_pointer<PtrItemType>::type ItemType;
			typedef std::unique_ptr<ItemType> ManagedItemType;
			typedef Utils::CopyByPtrCloneWrapper<ManagedItemType> CopyableItemType;
			typedef CloneableContainers::RemovableVector<CopyableItemType> ContainerType;

		public:
			typedef typename ContainerType::Identifier Identifier;
			typedef typename ContainerType::IdentifierHasher IdentifierHasher;

			template <typename T>
			Identifier PushBack(T&& item)
			{
				return container_.PushBack(CopyableItemType(std::forward<T>(item)));
			}

			const PtrItemType Get(Identifier identifier) const
			{
				auto ret = container_.Get(identifier);
				if (!ret) return nullptr;
				return ret->Get().get();
			}

			PtrItemType Get(Identifier identifier)
			{
				auto ret = container_.Get(identifier);
				if (!ret) return nullptr;
				return ret->Get().get();
			}

			void Remove(Identifier identifier) {
				container_.Remove(identifier);
			}

			void Clear() {
				container_.Clear();
			}

		public: // iterate
			template <typename IterateCallback> // bool(PtrItemType), return true to continue; false to abort
			void IterateAll(const IterateCallback & callback)
			{
				container_.IterateAll([&callback](CopyableItemType & item) -> bool {
					return callback(item.Get().get());
				});
			}

		private:
			ContainerType container_;
		};
	}
}