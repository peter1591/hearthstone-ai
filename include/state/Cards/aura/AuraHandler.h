#pragma once

namespace FlowControl
{
	namespace Context
	{
		struct AuraIsValid;
		struct AuraGetTargets;
		struct AuraApplyOn;
		struct AuraRemoveFrom;
	}
}

namespace state {
	namespace utils { class TargetsGenerator; }

	namespace Cards {
		namespace aura {
			class AuraHandler
			{
			public:
				typedef bool FuncIsValid(FlowControl::Context::AuraIsValid && context);
				typedef void FuncGetTargets(FlowControl::Context::AuraGetTargets && context);
				typedef void FuncApplyOn(FlowControl::Context::AuraApplyOn && context);
				typedef void FuncRemoveFrom(FlowControl::Context::AuraRemoveFrom && context);

				AuraHandler() : is_valid(nullptr), get_targets(nullptr), apply_on(nullptr), remove_from(nullptr)
				{
				}

				FuncIsValid * is_valid;
				FuncGetTargets * get_targets;
				FuncApplyOn * apply_on;
				FuncRemoveFrom * remove_from;
			};
		}
	}
}