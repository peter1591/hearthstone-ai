#pragma once

#include <map>

namespace FlowControl
{
	namespace Helpers
	{
		class Resolver;
	}
	class IRandomGenerator;
	class ActionParameterWrapper;

	class FlowContext
	{
		friend class Helpers::Resolver;
	public:
		FlowContext(IRandomGenerator & random, ActionParameterWrapper & action_parameters)
			: random_(random), action_parameters_(action_parameters)
		{
			result_ = kResultNotDetermined;
		}

		void AddDeadEntryHint(state::State & state, state::CardRef ref)
		{
			int play_order = state.mgr.Get(ref).GetPlayOrder();

			dead_entity_hints_.insert(std::make_pair(play_order, ref));
		}

		bool Empty() const
		{
			if (!dead_entity_hints_.empty()) return false;
			return true;
		}

	public:
		IRandomGenerator & random_;
		ActionParameterWrapper & action_parameters_;
		Result result_;

	private:
		std::multimap<int, state::CardRef> dead_entity_hints_;
	};
}