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
			friend class Vector<ItemType>;
			friend class Vector::IdentifierHasher;

		public:
			Identifier(const Identifier & rhs) = default;
			Identifier(Identifier && rhs) = default;
			Identifier & operator=(const Identifier & rhs) = default;
			Identifier & operator=(Identifier && rhs) = default;

			static Identifier GetInvalidIdentifier() { return Identifier(-1); }

		private:
			explicit Identifier(int idx) : idx(idx) {}

			int idx;

		public:
			bool operator==(const Identifier& rhs) const { return idx == rhs.idx; }
			bool operator!=(const Identifier& rhs) const { return idx != rhs.idx; }
			bool IsValid() const { return idx >= 0; }
		};

		class IdentifierHasher
		{
		public:
			size_t operator()(const Identifier & id) const { return std::hash<decltype(id.idx)>()(id.idx); }
		};

		Vector() {}
		Vector(size_t default_capacity) {
			items_.reserve(default_capacity);
		}

	public:
		template <typename T>
		Identifier PushBack(T&& item) {
			Identifier ret((int)items_.size());
			items_.push_back(std::forward<T>(item));
			return ret;
		}

		template <typename T>
		const ItemType & Get(T&& identifier) const {
			static_assert(std::is_same<std::decay<T>::type, Identifier>::value, "Wrong type");

			return items_[identifier.idx];
		}

		template <typename T>
		ItemType & Get(T&& identifier) {
			static_assert(std::is_same<std::decay<T>::type, Identifier>::value, "Wrong type");

			return items_[identifier.idx];
		}

	public: // iterate
		Identifier GetBegin() { return Identifier(0); }
		void StepNext(Identifier & id) { ++id.idx; }
		Identifier GetEnd() { return Identifier((int)items_.size()); }

	private:
		std::vector<ItemType> items_;
	};

}