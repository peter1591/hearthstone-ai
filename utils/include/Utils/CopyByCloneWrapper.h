#pragma once

namespace Utils
{
	template <typename T>
	class CopyByCloneWrapper
	{
	public:
		CopyByCloneWrapper(T&& item) : item_(std::move(item)) {}
		CopyByCloneWrapper(const CopyByCloneWrapper<T>& rhs)
			: item_(rhs.item_.Clone())
		{
		}
		CopyByCloneWrapper(CopyByCloneWrapper<T>&& rhs) = default;

		CopyByCloneWrapper& operator=(const CopyByCloneWrapper<T>& rhs)
		{
			item_ = std::move(item_.Clone());
			return *this;
		}
		CopyByCloneWrapper& operator=(CopyByCloneWrapper<T>&& rhs) = default;

		T & Get() { return item_; }
		const T & Get() const { return item_; }

	private:
		T item_;
	};

	template <typename T>
	class CopyByPtrCloneWrapper
	{
	public:
		CopyByPtrCloneWrapper(T&& item) : item_(std::move(item)) {}
		CopyByPtrCloneWrapper(const CopyByPtrCloneWrapper<T>& rhs)
			: item_(rhs.item_->Clone())
		{
		}
		CopyByPtrCloneWrapper(CopyByPtrCloneWrapper<T>&& rhs) = default;

		CopyByPtrCloneWrapper& operator=(const CopyByPtrCloneWrapper<T>& rhs)
		{
			item_ = std::move(item_->Clone());
			return *this;
		}
		CopyByPtrCloneWrapper& operator=(CopyByPtrCloneWrapper<T>&& rhs) = default;

		T & Get() { return item_; }
		const T & Get() const { return item_; }

	private:
		T item_;
	};
}