#pragma once

template <typename T>
class CloneByCopyWrapper
{
public:
	CloneByCopyWrapper(T&& item) : item_(std::move(item)) {}
	CloneByCopyWrapper(const CloneByCopyWrapper<T>& rhs)
		: item_(rhs.item_.Clone())
	{
	}
	CloneByCopyWrapper(CloneByCopyWrapper<T>&& rhs) = default;

	CloneByCopyWrapper& operator=(const CloneByCopyWrapper<T>& rhs)
	{
		item_ = std::move(item_.Clone())
		return *this;
	}
	CloneByCopyWrapper& operator=(CloneByCopyWrapper<T>&& rhs) = default;

	T & Get() { return item_; }
	const T & Get() const { return item_; }

private:
	T item_;
};