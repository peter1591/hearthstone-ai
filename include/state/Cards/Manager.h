#pragma once

#include <optional>
#include <utility>

#include "Utils/CloneableContainers/Vector.h"
#include "Utils/NeverShrinkVector.h"
#include "state/Cards/Card.h"
#include "state/Types.h"

namespace state
{
	namespace Cards
	{
		class Manager
		{
		public:
			using ItemType = std::optional<Card>;

			// A customized vector is used to ensure the underlying buffer only grows, never shrinks
			typedef Utils::CloneableContainers::Vector<ItemType, Utils::NeverShrinkVector<ItemType>> ContainerType;

			Manager() : base_(nullptr), cards_() {}

			Manager(Manager const& rhs) :
				base_(rhs.base_), cards_(rhs.cards_)
			{}

			void FillWithBase(Manager const& base) {
				assert(base.base_ == nullptr);
				base_ = &base;
				cards_.Resize(base.cards_.Size());
			}

			Manager & operator=(Manager const& rhs) {
				base_ = rhs.base_;
				cards_ = rhs.cards_;
				return *this;
			}

		public:
			Card const& Get(CardRef id) const {
				auto const& item = cards_.Get(id.id);
				if (item.has_value()) return *item;

				assert(base_);
				return base_->Get(id);
			}

			Card & GetMutable(CardRef id) {
				auto & item = cards_.Get(id.id);
				if (item.has_value()) return *item;

				assert(base_);
				item = base_->Get(id); // copy-on-write
				return *item;
			}

			CardRef PushBack(Cards::Card && card)
			{
				assert(card.GetZone() == kCardZoneNewlyCreated);
				return CardRef(cards_.PushBack(std::move(card)));
			}

			void SetCardZonePos(CardRef ref, int pos)
			{
				GetMutable(ref).SetZonePos()(pos);
			}

		private:
			Manager const* base_;
			ContainerType cards_;
		};
	}
}