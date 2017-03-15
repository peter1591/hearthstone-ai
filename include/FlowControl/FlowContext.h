#pragma once

#include <map>

#include "state/IRandomGenerator.h"
#include "state/Types.h"
#include "FlowControl/ActionParameterWrapper.h"

namespace FlowControl {
	namespace Helpers {
		class Resolver;
	}

	enum Result
	{
		kResultNotDetermined,
		kResultFirstPlayerWin,
		kResultSecondPlayerWin,
		kResultDraw,
		kResultInvalid
	};

	class FlowContext
	{
		friend class Helpers::Resolver;

	public:
		FlowContext(state::IRandomGenerator & random, ActionParameterWrapper & action_parameters);

		void AddDeadEntryHint(state::State & state, state::CardRef ref);
		bool Empty() const;

	public:
		state::IRandomGenerator & random_;
		ActionParameterWrapper & action_parameters_;
		Result result_;

		state::CardRef battlecry_target_;
		state::CardRef destroyed_weapon_;

	private:
		std::multimap<int, state::CardRef> dead_entity_hints_;
	};
}