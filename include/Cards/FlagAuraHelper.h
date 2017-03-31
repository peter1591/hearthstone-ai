#pragma once

#include "state/targetor/TargetsGenerator.h"
#include "FlowControl/aura/Handler.h"
#include "Cards/MinionCardUtils.h"

namespace Cards
{
	struct AliveWhenInPlay {
		static auto& GetRegisterCallback(state::Cards::CardData &card_data) {
			return card_data.added_to_play_zone;
		}

		template <typename Context>
		static bool IsAlive(Context&& context, state::CardRef card_ref) {
			state::Cards::Card const& card = context.maniulate_.Board().GetCard(card_ref);
			if (card.GetZone() != state::kCardZonePlay) return false;
			if (card.GetHP() <= 0) return false;
			return true;
		}
	};

	template <typename HandleClass, typename LifeTime>
	class FlagAuraHelper
	{
	public:
		FlagAuraHelper(state::Cards::CardData & card_data)
		{
			LifeTime::GetRegisterCallback() += [](state::Cards::ZoneChangedContext&& context) {
				FlowControl::flag_aura::Handler handler;

				assert(!handler.IsCallbackSet_IsValid());
				handler.SetCallback_IsValid([](auto context) {
					if (!LifeTime::IsAlive(context, context.card_ref_)) return false;
					return true;
				});

				assert(!handler.IsCallbackSet_Apply());
				handler.SetCallback_Apply(HandleClass::FlagAuraApply);
				assert(!handler.IsCallbackSet_Remove());
				handler.SetCallback_Remove(HandleClass::FlagAuraRemove);

				context.manipulate_.FlagAura().Add(context.card_ref_, std::move(handler));
			};
		}
	};


	// flag aura on a specific player
	template <typename HandleClass>
	struct OwnerPlayerFlagHandleClassWrapper {
		static void FlagAuraApply(FlowControl::flag_aura::contexts::AuraApply context) {
			auto const& player = context.card_.GetPlayerIdentifier();
			if (context.handler_.applied_player == player) return;
			FlagAuraRemove(context);
			context.handler_.applied_player = context.card_.GetPlayerIdentifier();
			HandleClass::PlayerFlagAuraApply(
				context.manipulate_.Board().Player(context.handler_.applied_player));
		}
		static void FlagAuraRemove(FlowControl::flag_aura::contexts::AuraRemove context) {
			auto& player = context.handler_.applied_player;
			if (!player.IsValid()) return;
			HandleClass::PlayerFlagAuraRemove(
				context.manipulate_.Board().Player(player));
			player.InValidate();
		}
	};

	template <typename HandleClass, typename... Types>
	using PlayerFlagAuraHelper = FlagAuraHelper<OwnerPlayerFlagHandleClassWrapper<HandleClass>, Types...>;
}