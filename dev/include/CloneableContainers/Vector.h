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
	template <typename ItemType> class Vector;
	class VectorIdentifierHasher;

	class VectorIdentifier
	{
		template <typename ItemType> friend class Vector;
		friend class VectorIdentifierHasher;

	public:
		VectorIdentifier(const VectorIdentifier & rhs) = default;
		VectorIdentifier(VectorIdentifier && rhs) = default;
		VectorIdentifier & operator=(const VectorIdentifier & rhs) = default;
		VectorIdentifier & operator=(VectorIdentifier && rhs) = default;

		static VectorIdentifier GetInvalidIdentifier() { return VectorIdentifier(-1); }

	private:
		explicit VectorIdentifier(int idx) : idx(idx) {}

		int idx;

	public:
		bool operator==(const VectorIdentifier& rhs) const { return idx == rhs.idx; }
		bool operator!=(const VectorIdentifier& rhs) const { return idx != rhs.idx; }
		bool IsValid() const { return idx >= 0; }
	};

	class VectorIdentifierHasher
	{
	public:
		size_t operator()(const VectorIdentifier & id) const { return std::hash<decltype(id.idx)>()(id.idx); }
	};

	template <class ItemType> // pointer is not cloneable
	class Vector
	{
	public:
		typedef VectorIdentifier Identifier;

		Vector()
		{
			static_assert(!std::is_pointer<ItemType>::value, "Use PtrVector instead to store pointers.");
		}
		Vector(size_t default_capacity) {
			static_assert(!std::is_pointer<ItemType>::value, "Use PtrVector instead to store pointers.");
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