#pragma once

#include <functional>
#include <unordered_set>
#include "state/Types.h"

namespace state
{
	class State;
	namespace Cards {
		class Card;
		namespace aura { class AuraAuxData; }
	}
	namespace targetor {
		class TargetsGenerator;
	}
}

namespace FlowControl
{
	class FlowContext;
}