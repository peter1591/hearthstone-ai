#pragma once

#include <utility>
#include <memory>
#include <type_traits>
#include "CloneableContainers/Vector.h"
#include "Utils/CopyByCloneWrapper.h"

namespace CloneableContainers
{
	template <typename PtrItemType,
			  typename std::enable_if<std::is_pointer<PtrItemType>::value, int>::type = 0>
	class PtrVector
	{
	private:
		typedef typename std::remove_pointer<PtrItemType>::type ItemType;
		typedef std::unique_ptr<ItemType> ManagedItemType;
		typedef Utils::CopyByPtrCloneWrapper<ManagedItemType> CopyableItemType;
		typedef CloneableContainers::Vector<CopyableItemType> ContainerType;

	public:
		typedef typename ContainerType::Identifier Identifier;

		template <typename T>
		Identifier PushBack(T&& item)
		{
			return container_.PushBack(CopyableItemType(std::forward<T>(item)));
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
		Identifier GetBegin() { return container_.GetBegin(); }
		void StepNext(Identifier & id) { container_.StepNext(id); }
		Identifier GetEnd() { return container_.GetEnd(); }

	private:
		ContainerType container_;
	};
}