#pragma once

#include <utility>
#include <vector>
#include <type_traits>

// This is a cloneable container, since the identifier is defined to be the index
// On the contrary, if we use raw pointer as the identifier, those identifiers should be
//    changed as well after cloned.
// BTW, this comes with a price: items cannot be erased/removed

namespace Utils
{
	namespace CloneableContainers
	{
		template <typename ItemType, class Container> class Vector;

		class VectorIdentifier
		{
			template <class ItemType, class Container> friend class Vector;

		public:
			constexpr VectorIdentifier() : idx(-1) {}
			VectorIdentifier(const VectorIdentifier & rhs) = default;
			VectorIdentifier(VectorIdentifier && rhs) = default;
			VectorIdentifier & operator=(const VectorIdentifier & rhs) = default;
			VectorIdentifier & operator=(VectorIdentifier && rhs) = default;
			operator int() const { return idx; }

			static constexpr VectorIdentifier GetInvalidIdentifier() { return VectorIdentifier(); }

		private:
			explicit VectorIdentifier(int idx) : idx(idx) {}

			int idx;

		public:
			bool operator==(const VectorIdentifier& rhs) const { return idx == rhs.idx; }
			bool operator!=(const VectorIdentifier& rhs) const { return idx != rhs.idx; }
			bool IsValid() const { return idx >= 0; }
		};

		template <class ItemType,
			class Container = std::vector<ItemType>>
		class Vector
		{
		public:
			typedef VectorIdentifier Identifier;

			Vector() : items_() {}

			// fill
			Vector(size_t size) : items_(size) {}

		public:
			template <typename T>
			Identifier PushBack(T&& item) {
				Identifier ret((int)items_.size());
				items_.push_back(std::forward<T>(item));
				return ret;
			}

			const ItemType & Get(Identifier identifier) const {
				return items_[identifier.idx];
			}

			ItemType & Get(Identifier identifier) {
				return items_[identifier.idx];
			}

			void Reset() {
				// All identifiers are invalidated
				items_.clear();
			}

			size_t Size() const { return items_.size(); }
			void Resize(size_t size) {
				items_.resize(size);
			}

			Identifier GetNextPushBackItemIdentifier() const { return Identifier((int)items_.size()); }
			Identifier GetNextNextPushBackItemIdentifier() const { return Identifier((int)items_.size() + 1); }

		public: // iterate
			Identifier GetBegin() { return Identifier(0); }
			void StepNext(Identifier & id) { ++id.idx; }
			Identifier GetEnd() { return Identifier((int)items_.size()); }

			template <typename IterateCallback> // bool(ItemType&), return true to continue; false to abort
			void IterateAll(IterateCallback&& callback) {
				for (ItemType & item : items_) {
					if (!callback(item)) return;
				}
			}
			template <typename IterateCallback> // bool(ItemType const&), return true to continue; false to abort
			void IterateAll(IterateCallback&& callback) const{
				for (ItemType & item : items_) {
					if (!callback(item)) return;
				}
			}

		private:
			Container items_;
		};
	}
}

namespace std
{
	template <>
	struct hash<Utils::CloneableContainers::VectorIdentifier>
	{
		std::size_t operator()(const Utils::CloneableContainers::VectorIdentifier& key) const
		{
			return std::hash<int>()(key);
		}
	};
}
