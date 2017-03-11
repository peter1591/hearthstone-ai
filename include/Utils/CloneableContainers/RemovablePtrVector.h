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

			template <typename T>
			const PtrItemType Get(T&& identifier) const
			{
				auto ret = container_.Get(std::forward<T>(identifier));
				if (!ret) return nullptr;
				return ret->Get().get();
			}

			template <typename T>
			PtrItemType Get(T&& identifier)
			{
				auto ret = container_.Get(std::forward<T>(identifier));
				if (!ret) return nullptr;
				return ret->Get().get();
			}

			template <typename T>
			void Remove(T&& identifier) {
				container_.Remove(std::forward<T>(identifier));
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