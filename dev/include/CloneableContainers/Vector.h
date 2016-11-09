#pragma once

#include <utility>
#include <vector>
#include <type_traits>

// This is a cloneable container, since the identifier is defined to be the index
// On the contrary, if we use raw pointer as the identifier, those identifiers should be
//    changed as well after cloned.
// BTW, this comes with a price: items cannot be erased/removed

namespace CloneableContainers
{
	template <class ItemType,
			  typename std::enable_if<!std::is_pointer<ItemType>::value, int>::type = 0> // pointer is not cloneable
	class Vector
	{
	public:
		class Identifier
		{
			friend Vector<ItemType>;

			explicit Identifier(size_t idx) : idx(idx) {}
			size_t idx;

		public:
			bool operator==(const Identifier& rhs) const { return idx == rhs.idx; }
			bool operator!=(const Identifier& rhs) const { return idx != rhs.idx; }
		};

		Vector() {}
		Vector(size_t default_capacity) {
			items_.reserve(default_capacity);
		}

		template <typename T>
		Identifier PushBack(T&& item) {
			Identifier ret(items_.size());
			items_.push_back(std::forward<T>(item));
			return ret;
		}

		const ItemType & Get(Identifier identifier) const {
			return items_[identifier.idx];
		}

		ItemType & Get(Identifier identifier) {
			return items_[identifier.idx];
		}

	public: // iterate
		Identifier GetBegin() { return Identifier(0); }
		void StepNext(Identifier & id) { ++id.idx; }
		Identifier GetEnd() { return Identifier(items_.size()); }

	private:
		std::vector<ItemType> items_;
	};

}