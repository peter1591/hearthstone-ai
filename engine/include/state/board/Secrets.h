#pragma once

#include <string>
#include <unordered_map>
#include "state/Types.h"
#include "Cards/id-map.h"

namespace state
{
	namespace detail { template <CardType TargetCardType, CardZone TargetCardZone> struct PlayerDataStructureMaintainer; }

	namespace board
	{
		class Secrets
		{
		private:
			using ContainerType = std::unordered_map<int, CardRef>;

		public:
			Secrets() : base_(nullptr), secrets_() {}

			Secrets(Secrets const& rhs) : base_(rhs.base_), secrets_(rhs.secrets_) {}
			Secrets & operator=(Secrets const& rhs) {
				base_ = rhs.base_;
				secrets_ = rhs.secrets_;
				return *this;
			}

			void FillWithBase(Secrets const& base) {
				assert(base.base_ == nullptr);
				base_ = &base.secrets_;
			}

			bool Exists(::Cards::CardId card_id) const
			{
				ContainerType const& container = GetContainer();
				return container.find((int)card_id) != container.end();
			}

			template <typename Functor>
			void ForEach(Functor&& functor) const {
				ContainerType const& container = GetContainer();
				for (auto const& kv : container) {
					functor(kv.second);
				}
			}

			bool Empty() const { return GetContainer().empty(); }

			void Add(::Cards::CardId card_id, CardRef card)
			{
				if (Exists(card_id)) throw std::runtime_error("Secret already exists");
				GetContainerForWrite().insert(std::make_pair((int)card_id, card));
			}

			void Remove(::Cards::CardId card_id)
			{
				GetContainerForWrite().erase(card_id);
			}

			void Clear()
			{
				GetContainerForWrite().clear();
			}

		private:
			ContainerType & GetContainerForWrite() {
				if (base_) {
					secrets_ = *base_;
					base_ = nullptr;
				}
				return secrets_;
			}

			ContainerType const& GetContainer() const {
				if (base_) return *base_;
				else return secrets_;
			}

		private:
			ContainerType const* base_;
			ContainerType secrets_;
		};
	}
}