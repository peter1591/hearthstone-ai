#pragma once

#include "state/Cards/aura/Contexts.h"

namespace state {
	namespace Cards {
		namespace aura {
			class AuraHandler
			{
			public:
				typedef bool FuncIsValid(contexts::AuraIsValid context);
				typedef void FuncGetTargets(contexts::AuraGetTargets context);
				typedef void FuncApplyOn(contexts::AuraApplyOn context);
				typedef void FuncRemoveFrom(contexts::AuraRemoveFrom context);

				AuraHandler() : is_valid(nullptr), get_targets(nullptr), apply_on(nullptr), remove_from(nullptr) {}

				FuncIsValid * is_valid;
				FuncGetTargets * get_targets;
				FuncApplyOn * apply_on;
				FuncRemoveFrom * remove_from;
			};
		}
	}
}