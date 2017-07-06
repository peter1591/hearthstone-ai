#pragma once

#include <assert.h>
#include "state/Types.h"
#include "state/board/Board.h"
#include "state/Cards/Manager.h"
#include "state/detail/PlayerDataStructureMaintainer.h"
#include "state/detail/InvokeCallback.h"

namespace state {
	template <CardZone ChangingCardZone, CardType ChangingCardType>
	class ZoneChanger
	{
	public:
		ZoneChanger(State & state, board::Board & board, Cards::Manager & cards_mgr,CardRef card_ref)
			: state_(state), board_(board), cards_mgr_(cards_mgr), card_ref_(card_ref)
		{
			assert(cards_mgr_.Get(card_ref_).GetZone() == ChangingCardZone);
			assert(cards_mgr_.Get(card_ref_).GetCardType() == ChangingCardType);
		}

		template <CardZone ChangeToZone,
			typename = std::enable_if_t<detail::PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::SpecifyAddPosition == false>>
			void ChangeTo(PlayerIdentifier player_identifier)
		{
			constexpr bool zone_changed = (ChangingCardZone != ChangeToZone);

			detail::PlayerDataStructureMaintainer<ChangingCardType, ChangingCardZone>::Remove(board_, cards_mgr_, card_ref_);
			if (zone_changed) detail::InvokeCallback<ChangingCardType, ChangingCardZone>::Removed(state_, card_ref_);
			cards_mgr_.GetMutable(card_ref_).SetZone()(player_identifier, ChangeToZone);
			detail::PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::Add(board_, cards_mgr_, card_ref_);
			if (zone_changed) detail::InvokeCallback<ChangingCardType, ChangeToZone>::Added(state_, card_ref_);
		}

		template <CardZone ChangeToZone,
			typename = std::enable_if_t<detail::PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::SpecifyAddPosition == true>>
			void ChangeTo(PlayerIdentifier player_identifier, int pos)
		{
			constexpr bool zone_changed = (ChangingCardZone != ChangeToZone);

			detail::PlayerDataStructureMaintainer<ChangingCardType, ChangingCardZone>::Remove(board_, cards_mgr_, card_ref_);
			if (zone_changed) detail::InvokeCallback<ChangingCardType, ChangingCardZone>::Removed(state_, card_ref_);
			cards_mgr_.GetMutable(card_ref_).SetZone()(player_identifier, ChangeToZone);
			detail::PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::Add(board_, cards_mgr_, card_ref_, pos);
			if (zone_changed) detail::InvokeCallback<ChangingCardType, ChangeToZone>::Added(state_, card_ref_);
		}

		void ReplaceBy(CardRef new_ref)
		{
			assert(cards_mgr_.Get(new_ref).GetZone() == kCardZoneNewlyCreated
				|| cards_mgr_.Get(new_ref).GetZone() == kCardZoneSetASide
			);
			assert(cards_mgr_.Get(card_ref_).GetCardType() == cards_mgr_.Get(new_ref).GetCardType());

			detail::PlayerDataStructureMaintainer<ChangingCardType, ChangingCardZone>::ReplaceBy(board_, cards_mgr_, card_ref_, new_ref);

			cards_mgr_.GetMutable(new_ref).SetZone()(cards_mgr_.Get(card_ref_).GetPlayerIdentifier(), cards_mgr_.Get(card_ref_).GetZone());
			cards_mgr_.GetMutable(new_ref).SetZonePos()(cards_mgr_.Get(card_ref_).GetZonePosition());
			cards_mgr_.GetMutable(card_ref_).SetZone().Zone(kCardZoneSetASide);

			detail::InvokeCallback<ChangingCardType, ChangingCardZone>::Removed(state_, card_ref_);
			detail::InvokeCallback<ChangingCardType, ChangingCardZone>::Added(state_, new_ref);
		}

	private:
		State & state_;
		board::Board& board_;
		Cards::Manager& cards_mgr_;
		CardRef card_ref_;
	};

	template <CardType ChangingCardType>
	class ZoneChangerWithUnknownZone
	{
	public:
		ZoneChangerWithUnknownZone(State & state, board::Board & board, Cards::Manager & cards_mgr,CardRef card_ref)
			: state_(state), board_(board), cards_mgr_(cards_mgr), card_ref_(card_ref)
		{
			assert(cards_mgr.Get(card_ref_).GetCardType() == ChangingCardType);
		}

		template <CardZone ChangeToZone,
			typename = std::enable_if_t<detail::PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::SpecifyAddPosition == false>>
			void ChangeTo(PlayerIdentifier player_identifier)
		{
			switch (cards_mgr_.Get(card_ref_).GetZone())
			{
			case kCardZoneGraveyard:
				ZoneChanger<kCardZoneGraveyard, ChangingCardType>(state_, board_, cards_mgr_, card_ref_).ChangeTo<ChangeToZone>(player_identifier);
				return;
			case kCardZoneHand:
				ZoneChanger<kCardZoneHand, ChangingCardType>(state_, board_, cards_mgr_, card_ref_).ChangeTo<ChangeToZone>(player_identifier);
				return;
			case kCardZonePlay:
				ZoneChanger<kCardZonePlay, ChangingCardType>(state_, board_, cards_mgr_, card_ref_).ChangeTo<ChangeToZone>(player_identifier);
				return;
			case kCardZoneSetASide:
				ZoneChanger<kCardZoneSetASide, ChangingCardType>(state_, board_, cards_mgr_, card_ref_).ChangeTo<ChangeToZone>(player_identifier);
				return;
			case kCardZoneRemoved:
				ZoneChanger<kCardZoneRemoved, ChangingCardType>(state_, board_, cards_mgr_, card_ref_).ChangeTo<ChangeToZone>(player_identifier);
				return;
			case kCardZoneInvalid:
				ZoneChanger<kCardZoneInvalid, ChangingCardType>(state_, board_, cards_mgr_, card_ref_).ChangeTo<ChangeToZone>(player_identifier);
				return;
			case kCardZoneNewlyCreated:
				ZoneChanger<kCardZoneNewlyCreated, ChangingCardType>(state_, board_, cards_mgr_, card_ref_).ChangeTo<ChangeToZone>(player_identifier);
				return;
			default:
				throw std::runtime_error("Unknown card zone");
			}
		}

		template <CardZone ChangeToZone,
			typename = std::enable_if_t<detail::PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::SpecifyAddPosition == true>>
			void ChangeTo(PlayerIdentifier player_identifier, int pos)
		{
			switch (cards_mgr_.Get(card_ref_).GetZone())
			{
			case kCardZoneGraveyard:
				ZoneChanger<kCardZoneGraveyard, ChangingCardType>(state_, board_, cards_mgr_, card_ref_).ChangeTo<ChangeToZone>(player_identifier, pos);
				return;
			case kCardZoneHand:
				ZoneChanger<kCardZoneHand, ChangingCardType>(state_, board_, cards_mgr_, card_ref_).ChangeTo<ChangeToZone>(player_identifier, pos);
				return;
			case kCardZonePlay:
				ZoneChanger<kCardZonePlay, ChangingCardType>(state_, board_, cards_mgr_, card_ref_).ChangeTo<ChangeToZone>(player_identifier, pos);
				return;
			case kCardZoneSetASide:
				ZoneChanger<kCardZoneSetASide, ChangingCardType>(state_, board_, cards_mgr_, card_ref_).ChangeTo<ChangeToZone>(player_identifier, pos);
				return;
			case kCardZoneRemoved:
				ZoneChanger<kCardZoneRemoved, ChangingCardType>(state_, board_, cards_mgr_, card_ref_).ChangeTo<ChangeToZone>(player_identifier, pos);
				return;
			case kCardZoneInvalid:
				ZoneChanger<kCardZoneInvalid, ChangingCardType>(state_, board_, cards_mgr_, card_ref_).ChangeTo<ChangeToZone>(player_identifier, pos);
				return;
			case kCardZoneNewlyCreated:
				ZoneChanger<kCardZoneNewlyCreated, ChangingCardType>(state_, board_, cards_mgr_, card_ref_).ChangeTo<ChangeToZone>(player_identifier, pos);
				return;
			default:
				throw std::runtime_error("Unknown card zone");
			}
		}

	private:
		State & state_;
		board::Board& board_;
		Cards::Manager& cards_mgr_;
		CardRef card_ref_;
	};

	namespace detail {
		template <CardZone ChangingCardZone, CardZone ChangeToZone>
		struct ZoneChangerWithUnknownType {
			static void ChangeTo(State & state_, board::Board & board_, Cards::Manager & cards_mgr_, CardRef card_ref_,
				PlayerIdentifier player_identifier)
			{
				switch (cards_mgr_.Get(card_ref_).GetCardType())
				{
				case kCardTypeHero:
					ZoneChanger<ChangingCardZone, kCardTypeHero>(state_, board_, cards_mgr_, card_ref_)
						.template ChangeTo<ChangeToZone>(player_identifier);
					return;
				case kCardTypeHeroPower:
					ZoneChanger<ChangingCardZone, kCardTypeHeroPower>(state_, board_, cards_mgr_, card_ref_)\
						.template ChangeTo<ChangeToZone>(player_identifier);
					return;
				case kCardTypeMinion:
					ZoneChanger<ChangingCardZone, kCardTypeMinion>(state_, board_, cards_mgr_, card_ref_)
						.template ChangeTo<ChangeToZone>(player_identifier);
					return;
				case kCardTypeSpell:
					ZoneChanger<ChangingCardZone, kCardTypeSpell>(state_, board_, cards_mgr_, card_ref_)
						.template ChangeTo<ChangeToZone>(player_identifier);
					return;
				case kCardTypeWeapon:
					ZoneChanger<ChangingCardZone, kCardTypeWeapon>(state_, board_, cards_mgr_, card_ref_)
						.template ChangeTo<ChangeToZone>(player_identifier);
					return;
				case kCardTypeEnchantment:
					ZoneChanger<ChangingCardZone, kCardTypeEnchantment>(state_, board_, cards_mgr_, card_ref_)
						.template ChangeTo<ChangeToZone>(player_identifier);
					return;
				default:
					throw std::runtime_error("unknown card type");
				}
			}

			static void ChangeTo(State & state_, board::Board & board_, Cards::Manager & cards_mgr_, CardRef card_ref_,
				PlayerIdentifier player_identifier, int pos)
			{
				switch (cards_mgr_.Get(card_ref_).GetCardType())
				{
				case kCardTypeHero:
					ZoneChanger<ChangingCardZone, kCardTypeHero>(state_, board_, cards_mgr_, card_ref_)
						.template ChangeTo<ChangeToZone>(player_identifier, pos);
					return;
				case kCardTypeHeroPower:
					ZoneChanger<ChangingCardZone, kCardTypeHeroPower>(state_, board_, cards_mgr_, card_ref_)
						.template ChangeTo<ChangeToZone>(player_identifier, pos);
					return;
				case kCardTypeMinion:
					ZoneChanger<ChangingCardZone, kCardTypeMinion>(state_, board_, cards_mgr_, card_ref_)
						.template ChangeTo<ChangeToZone>(player_identifier, pos);
					return;
				case kCardTypeSpell:
					ZoneChanger<ChangingCardZone, kCardTypeSpell>(state_, board_, cards_mgr_, card_ref_)
						.template ChangeTo<ChangeToZone>(player_identifier, pos);
					return;
				case kCardTypeWeapon:
					ZoneChanger<ChangingCardZone, kCardTypeWeapon>(state_, board_, cards_mgr_, card_ref_)
						.template ChangeTo<ChangeToZone>(player_identifier, pos);
					return;
				case kCardTypeEnchantment:
					ZoneChanger<ChangingCardZone, kCardTypeEnchantment>(state_, board_, cards_mgr_, card_ref_)
						.template ChangeTo<ChangeToZone>(player_identifier, pos);
					return;
				default:
					throw std::runtime_error("unknown card type");
				}
			}
		};

		template <CardZone ChangingCardZone>
		struct ZoneChangerWithUnknownType<ChangingCardZone, kCardZonePlay> {
			static void ChangeTo(State & state_, board::Board & board_, Cards::Manager & cards_mgr_, CardRef card_ref_,
				PlayerIdentifier player_identifier)
			{
				static constexpr CardZone ChangeToZone = kCardZonePlay;
				switch (cards_mgr_.Get(card_ref_).GetCardType())
				{
				case kCardTypeHero:
					ZoneChanger<ChangingCardZone, kCardTypeHero>(state_, board_, cards_mgr_, card_ref_)
						.template ChangeTo<ChangeToZone>(player_identifier);
					return;
				case kCardTypeHeroPower:
					ZoneChanger<ChangingCardZone, kCardTypeHeroPower>(state_, board_, cards_mgr_, card_ref_)
						.template ChangeTo<ChangeToZone>(player_identifier);
					return;
				case kCardTypeSpell:
					ZoneChanger<ChangingCardZone, kCardTypeSpell>(state_, board_, cards_mgr_, card_ref_)
						.template ChangeTo<ChangeToZone>(player_identifier);
					return;
				case kCardTypeWeapon:
					ZoneChanger<ChangingCardZone, kCardTypeWeapon>(state_, board_, cards_mgr_, card_ref_)
						.template ChangeTo<ChangeToZone>(player_identifier);
					return;
				case kCardTypeEnchantment:
					ZoneChanger<ChangingCardZone, kCardTypeEnchantment>(state_, board_, cards_mgr_, card_ref_)
						.template ChangeTo<ChangeToZone>(player_identifier);
					return;
				case kCardTypeMinion:
					assert(false); // should specify position
				default:
					throw std::runtime_error("unknown card type");
				}
			}

			static void ChangeTo(State & state_, board::Board & board_, Cards::Manager & cards_mgr_, CardRef card_ref_,
				PlayerIdentifier player_identifier, int pos)
			{
				static constexpr CardZone ChangeToZone = kCardZonePlay;
				switch (cards_mgr_.Get(card_ref_).GetCardType())
				{
				case kCardTypeMinion:
					return ZoneChanger<ChangingCardZone, kCardTypeMinion>(state_, board_, cards_mgr_, card_ref_)
						.template ChangeTo<ChangeToZone>(player_identifier, pos);
				case kCardTypeHero:
					assert(false); // should not specify position
				case kCardTypeHeroPower:
					assert(false); // should not specify position
				case kCardTypeSpell:
					assert(false); // should not specify position
				case kCardTypeWeapon:
					assert(false); // should not specify position
				case kCardTypeEnchantment:
					assert(false); // should not specify position
				default:
					throw std::runtime_error("unknown card type");
				}
			}
		};
	}

	template <CardZone ChangingCardZone>
	class ZoneChangerWithUnknownType
	{
	public:
		ZoneChangerWithUnknownType(State & state, board::Board & board, Cards::Manager & cards_mgr,CardRef card_ref)
			: state_(state), board_(board), cards_mgr_(cards_mgr), card_ref_(card_ref)
		{
			assert(cards_mgr.Get(card_ref_).GetZone() == ChangingCardZone);
		}

		template <CardZone ChangeToZone>
		void ChangeTo(PlayerIdentifier player_identifier)
		{
			return detail::ZoneChangerWithUnknownType<ChangingCardZone, ChangeToZone>::ChangeTo(
				state_, board_, cards_mgr_, card_ref_, player_identifier);
		}

		template <CardZone ChangeToZone>
		void ChangeTo(PlayerIdentifier player_identifier, int pos)
		{
			return detail::ZoneChangerWithUnknownType<ChangingCardZone, ChangeToZone>::ChangeTo(
				state_, board_, cards_mgr_, card_ref_, player_identifier, pos);
		}

	private:
		State & state_;
		board::Board& board_;
		Cards::Manager& cards_mgr_;
		CardRef card_ref_;
	};

	class ZoneChangerWithUnknownZoneUnknownType
	{
	public:
		ZoneChangerWithUnknownZoneUnknownType(State & state, board::Board & board, Cards::Manager& cards_mgr,CardRef card_ref)
			: state_(state), board_(board), cards_mgr_(cards_mgr), card_ref_(card_ref)
		{}

		template <CardZone ChangeToZone>
		void ChangeTo(PlayerIdentifier player_identifier)
		{
			switch (cards_mgr_.Get(card_ref_).GetCardType())
			{
			case kCardTypeMinion:
				return ZoneChangerWithUnknownZone<kCardTypeMinion>(state_, board_, cards_mgr_, card_ref_)
					.template ChangeTo<ChangeToZone>(player_identifier);
			case kCardTypeHeroPower:
				return ZoneChangerWithUnknownZone<kCardTypeHeroPower>(state_, board_, cards_mgr_, card_ref_)
					.template ChangeTo<ChangeToZone>(player_identifier);
			case kCardTypeSpell:
				return ZoneChangerWithUnknownZone<kCardTypeSpell>(state_, board_, cards_mgr_, card_ref_)
					.template ChangeTo<ChangeToZone>(player_identifier);
			case kCardTypeWeapon:
				return ZoneChangerWithUnknownZone<kCardTypeWeapon>(state_, board_, cards_mgr_, card_ref_)
					.template ChangeTo<ChangeToZone>(player_identifier);
			default:
				throw std::runtime_error("unknown card type");
			}
		}

		template <CardZone ChangeToZone>
		void ChangeTo(PlayerIdentifier player_identifier, int pos)
		{
			switch (cards_mgr_.Get(card_ref_).GetCardType())
			{
			case kCardTypeHero:
				return ZoneChangerWithUnknownZone<kCardTypeHero>(state_, board_, cards_mgr_, card_ref_)
					.template ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardTypeMinion:
				return ZoneChangerWithUnknownZone<kCardTypeMinion>(state_, board_, cards_mgr_, card_ref_)
					.template ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardTypeHeroPower:
				return ZoneChangerWithUnknownZone<kCardTypeHeroPower>(state_, board_, cards_mgr_, card_ref_)
					.template ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardTypeSpell:
				return ZoneChangerWithUnknownZone<kCardTypeSpell>(state_, board_, cards_mgr_, card_ref_)
					.template ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardTypeWeapon:
				return ZoneChangerWithUnknownZone<kCardTypeWeapon>(state_, board_, cards_mgr_, card_ref_)
					.template ChangeTo<ChangeToZone>(player_identifier, pos);
			default:
				throw std::runtime_error("unknown card type");
			}
		}

	private:
		State & state_;
		board::Board& board_;
		Cards::Manager& cards_mgr_;
		CardRef card_ref_;
	};
}