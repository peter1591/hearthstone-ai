#pragma once

#include "State/State.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			class DeathrattlesHelper
			{
			public:
				DeathrattlesHelper(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) :
					state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card)
				{
				}

				template <typename T>
				void Add(T&& deathrattle) {
					card_.AddDeathrattle(std::forward<T>(deathrattle));
				}

				DeathrattlesHelper & TriggerAll() {
					for (auto deathrattle : card_.MutableDeathrattles()) {
						deathrattle(FlowControl::Context::Deathrattle(state_, flow_context_, card_ref_, card_));
					}
					return *this;
				}

				DeathrattlesHelper & Clear() {
					card_.MutableDeathrattles().clear();
					return *this;
				}

			private:
				state::State & state_;
				FlowContext & flow_context_;
				state::CardRef card_ref_;
				state::Cards::Card & card_;
			};
		}
	}
}