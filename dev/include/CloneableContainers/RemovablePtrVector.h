#pragma once

#include <memory>
#include <type_traits>
#include "CloneableContainers/RemovableVector.h"
#include "CloneableContainers/impl/CopyByCloneWrapper.h"

namespace CloneableContainers
{
	template <typename PtrItemType,
		typename std::enable_if<std::is_pointer<PtrItemType>::value, int>::type = 0>
	class RemovablePtrVector
	{
	private:
		typedef typename std::remove_pointer<PtrItemType>::type ItemType;
		typedef std::unique_ptr<ItemType> ManagedItemType;
		typedef impl::CopyByPtrCloneWrapper<ManagedItemType> CopyableItemType;
		typedef CloneableContainers::RemovableVector<CopyableItemType> ContainerType;

	public:
		typedef typename ContainerType::Identifier Identifier;

		Identifier PushBack(ManagedItemType&& item)
		{
			return container_.PushBack(CopyableItemType(std::move(item)));
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

	public: // iterate
		Identifier GetFirst() { return container_.GetFirst(); }
		void StepNext(Identifier & id) { container_.StepNext(id); }
		bool ReachedEnd(const Identifier & id) { return container_.ReachedEnd(id); }

	private:
		ContainerType container_;
	};
}