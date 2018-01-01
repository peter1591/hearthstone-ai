#pragma once

#include <assert.h>
#include <functional>
#include <vector>
#include <variant>

#include "Cards/id-map.h"

namespace engine
{
	class ActionChoices
	{
	public:
		class InvalidChoice {
		public:
			int Get(size_t idx) const { throw std::exception(); }

			bool Empty() const { throw std::exception(); }

			int Size() const { throw std::exception(); }

			void Begin() { throw std::exception(); }

			int Get() const { throw std::exception(); }

			void StepNext() { throw std::exception(); }

			bool IsEnd() const { throw std::exception(); }
		};
		class ChooseFromZeroToExclusiveMax {
		public:
			ChooseFromZeroToExclusiveMax(int exclusive_max)
				: exclusive_max_(exclusive_max), it_(0)
			{}

			int Get(size_t idx) const {
				assert(idx < exclusive_max_);
				return (int)idx;
			}

			bool Empty() const { return exclusive_max_ <= 0; }

			int Size() const { return exclusive_max_; }

			void Begin() { it_ = 0; }

			int Get() const { return it_; }

			void StepNext() { ++it_; }

			bool IsEnd() const { return it_ >= exclusive_max_; }

		private:
			int exclusive_max_;
			int it_;
		};
		class ChooseFromCardIds {
		public:
			ChooseFromCardIds(std::vector< ::Cards::CardId> const& card_ids) :
				card_ids_(card_ids), it_()
			{}

			int Get(size_t idx) const {
				assert(idx < card_ids_.size());
				return (int)card_ids_[idx];
			}

			bool Empty() const { return card_ids_.empty(); }

			int Size() const { return (int)card_ids_.size(); }

			void Begin() { it_ = card_ids_.begin(); }

			int Get() const { return (int)*it_; }

			void StepNext() { ++it_; }

			bool IsEnd() const { return it_ == card_ids_.end(); }

		private:
			std::vector< ::Cards::CardId> card_ids_;
			std::vector< ::Cards::CardId>::const_iterator it_;
		};

	public:
		explicit ActionChoices(ChooseFromZeroToExclusiveMax const& v) : item_(v) {}

		explicit ActionChoices(ChooseFromCardIds const& v) : item_(v) {}

		ActionChoices() : item_(InvalidChoice()) {}

		template <class T>
		bool CheckType() const {
			return std::holds_alternative<T>(item_);
		}

		template <class Comparator>
		bool Compare(ActionChoices const& rhs, Comparator&& comparator) const {
			return std::visit([&](auto&& arg1, auto&& arg2) -> bool {
				return comparator(
					std::forward<decltype(arg1)>(arg1),
					std::forward<decltype(arg2)>(arg2));
			}, item_, rhs.item_);
		}

		size_t GetIndex() const { return item_.index(); }

		int Get(size_t idx) const {
			return std::visit([&](auto&& item) -> int {
				return item.Get(idx);
			}, item_);
		}

	public: // iterate
		bool Empty() const {
			return std::visit([&](auto&& item) {
				return item.Empty();
			}, item_);
		}

		int Size() const {
			return std::visit([&](auto&& item) {
				return item.Size();
			}, item_);
		}

		void Begin() {
			return std::visit([&](auto&& item) {
				return item.Begin();
			}, item_);
		}

		int Get() const {
			return std::visit([&](auto&& item) {
				return item.Get();
			}, item_);
		}

		void StepNext() {
			return std::visit([&](auto&& item) {
				return item.StepNext();
			}, item_);
		}

		bool IsEnd() const {
			return std::visit([&](auto&& item) {
				return item.IsEnd();
			}, item_);
		}

	private:
		using ItemType = std::variant<
			InvalidChoice,
			ChooseFromZeroToExclusiveMax,
			ChooseFromCardIds>;

		ItemType item_;
	};
}