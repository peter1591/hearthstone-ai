#pragma once

#include <utility>
#include <memory>
#include <type_traits>
#include "CloneableContainers/RemovableVector.h"
#include "Utils/CopyByCloneWrapper.h"

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
		Identifier GetBegin() { return container_.GetBegin(); }
		void StepNext(Identifier & id) { container_.StepNext(id); }
		Identifier GetEnd() { return container_.GetEnd(); }

	private:
		ContainerType container_;
	};
}