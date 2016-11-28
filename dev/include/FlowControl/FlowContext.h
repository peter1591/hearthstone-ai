#pragma once

#include <map>

namespace FlowControl
{
	namespace Helpers
	{
		class EntityDeathHandler;
	}

	class FlowContext
	{
		friend class Helpers::EntityDeathHandler;
	public:
		FlowContext()
		{
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

	private:
		std::multimap<int, state::CardRef> dead_entity_hints_;
	};
}