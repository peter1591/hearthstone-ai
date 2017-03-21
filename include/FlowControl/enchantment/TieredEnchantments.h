#pragma once

#include "FlowControl/enchantment/Enchantments.h"

namespace FlowControl
{
	namespace enchantment
	{
		enum EnchantmentTiers
		{
			kEnchantmentTier1,
			kEnchantmentTier2,
			kEnchantmentTier3
		};

		class TieredEnchantments
		{
		public:
			using ContainerType = Enchantments::ContainerType;
			using IdentifierType = std::pair<EnchantmentTiers, ContainerType::Identifier>;

			template <typename EnchantmentType>
			typename IdentifierType PushBack(state::State const& state)
			{
				EnchantmentType item;

				int valid_until_turn = -1;
				if (item.valid_this_turn) valid_until_turn = state.GetTurn();

				return std::make_pair(EnchantmentType::tier,
					GetEnchantments<EnchantmentType::tier>().PushBack(item.apply_functor, valid_until_turn));
			}

			void Remove(IdentifierType id)
			{
				switch (id.first) {
				case kEnchantmentTier1:
					return tier1_.Remove(id.second);
				case kEnchantmentTier2:
					return tier2_.Remove(id.second);
				case kEnchantmentTier3:
					return tier3_.Remove(id.second);
				}
				assert(false);
			}

			void Clear()
			{
				tier1_.Clear();
				tier2_.Clear();
				tier3_.Clear();
				aura_.Clear();
			}

			bool Exists(IdentifierType id) const
			{
				switch (id.first) {
				case kEnchantmentTier1:
					return tier1_.Exists(id.second);
				case kEnchantmentTier2:
					return tier2_.Exists(id.second);
				case kEnchantmentTier3:
					return tier3_.Exists(id.second);
				}
				assert(false);
				return false;
			}

			void ApplyAll(state::Cards::EnchantableStates & card, state::State const& state)
			{
				tier1_.ApplyAll(card, state);
				tier2_.ApplyAll(card, state);
				tier3_.ApplyAll(card, state);
			}

			bool NeedUpdate() const {
				if (tier1_.NeedUpdate()) return true;
				if (tier2_.NeedUpdate()) return true;
				if (tier3_.NeedUpdate()) return true;
				return false;
			}

			void FinishedUpdate() {
				tier1_.FinishedUpdate();
				tier2_.FinishedUpdate();
				tier3_.FinishedUpdate();
			}

		private:
			template <int Tier> Enchantments & GetEnchantments();
			template <int Tier> const Enchantments & GetEnchantments() const;

		private:
			Enchantments tier1_;
			Enchantments tier2_;
			Enchantments tier3_;
			Enchantments aura_;
		};

		template <> inline Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier1>() { return tier1_; }
		template <> inline const Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier1>() const { return tier1_; }
		template <> inline Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier2>() { return tier2_; }
		template <> inline const Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier2>() const { return tier2_; }
		template <> inline Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier3>() { return tier3_; }
		template <> inline const Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier3>() const { return tier3_; }
	}
}