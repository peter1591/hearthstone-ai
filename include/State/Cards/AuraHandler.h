#pragma once

namespace FlowControl
{
	namespace Context
	{
		class AuraGetTargets;
		class AuraApplyOn;
		class AuraRemoveFrom;
	}
}

namespace state
{
	namespace Cards
	{
		class AuraHandler
		{
		public:
			typedef void FuncGetTargets(FlowControl::Context::AuraGetTargets & context);
			typedef void FuncApplyOn(FlowControl::Context::AuraApplyOn & context);
			typedef void FuncRemoveFrom(FlowControl::Context::AuraRemoveFrom & context);

			AuraHandler() : get_targets(nullptr), apply_on(nullptr), remove_from(nullptr) {}

			FuncGetTargets * get_targets;
			FuncApplyOn * apply_on;
			FuncRemoveFrom * remove_from;
		};
	}
}