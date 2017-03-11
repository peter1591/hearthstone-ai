#pragma once

#include <map>

#include "state/ActionParameterWrapper.h"
#include "state/IRandomGenerator.h"
#include "state/State.h"
#include "state/Types.h"
#include "FlowControl/Result.h"

namespace FlowControl {
	namespace Helpers {
		class Resolver;
	}
}

namespace state
{
	class FlowContext
	{
		friend class FlowControl::Helpers::Resolver;

	public:
		FlowContext(IRandomGenerator & random, ActionParameterWrapper & action_parameters)
			: random_(random), action_parameters_(action_parameters)
		{
			result_ = FlowControl::kResultNotDetermined;
		}

		void AddDeadEntryHint(State & state, CardRef ref)
		{
			int play_order = state.mgr.Get(ref).GetPlayOrder();

			dead_entity_hints_.insert(std::make_pair(play_order, ref));
		}

		bool Empty() const
		{
			if (!dead_entity_hints_.empty()) return false;
			if (destroyed_weapon_.IsValid()) return false;
			return true;
		}

	public:
		IRandomGenerator & random_;
		ActionParameterWrapper & action_parameters_;
		FlowControl::Result result_;

		state::CardRef battlecry_target_;
		state::CardRef destroyed_weapon_;

	private:
		std::multimap<int, state::CardRef> dead_entity_hints_;
	};
}