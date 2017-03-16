#pragma once

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			class DeathrattlesHelper
			{
			public:
				DeathrattlesHelper(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) :
					state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card)
				{
				}

				template <typename T>
				void Add(T&& deathrattle) {
					card_.GetMutableDeathrattleHandlerGetter().Get().Add(std::forward<T>(deathrattle));
				}

				DeathrattlesHelper & TriggerAll() {
					card_.GetMutableDeathrattleHandlerGetter().Get().TriggerAll(state_, flow_context_, card_ref_, card_);
					return *this;
				}

				DeathrattlesHelper & Clear() {
					card_.GetMutableDeathrattleHandlerGetter().Get().Clear();
					return *this;
				}

			private:
				state::State & state_;
				FlowControl::FlowContext & flow_context_;
				state::CardRef card_ref_;
				state::Cards::Card & card_;
			};
		}
	}
}