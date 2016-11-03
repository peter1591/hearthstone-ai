#pragma once

#include <memory>
#include <type_traits>
#include "CloneableContainers/Vector.h"
#include "CloneableContainers/CopyByCloneWrapper.h"

namespace CloneableContainers
{
	template <typename PtrItemType,
			  typename std::enable_if<std::is_pointer<PtrItemType>::value, int>::type = 0>
	class PtrVector
	{
	private:
		typedef typename std::remove_pointer<PtrItemType>::type ItemType;
		typedef std::unique_ptr<ItemType> ManagedItemType;
		typedef CopyByPtrCloneWrapper<ManagedItemType> CopyableItemType;
		typedef CloneableContainers::Vector<CopyableItemType> ContainerType;

	public:
		typedef typename ContainerType::Identifier Identifier;

		Identifier PushBack(ManagedItemType&& item)
		{
			return container_.PushBack(CopyableItemType(std::move(item)));
		}

		const PtrItemType Get(Identifier identifier) const
		{
			return container_.Get(identifier).Get().get();
		}

		PtrItemType Get(Identifier identifier)
		{
			return container_.Get(identifier).Get().get();
		}

	public: // iterate
		Identifier GetFirst() { return container_.GetFirst(); }
		void StepNext(Identifier & id) { container_.StepNext(id); }
		bool ReachedEnd(const Identifier & id) { return container_.ReachedEnd(id); }

	private:
		ContainerType container_;
	};
}