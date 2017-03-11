#pragma once

namespace FlowControl
{
	namespace Helpers
	{
		class Resolver;
	}

	namespace Manipulators
	{
		namespace Helpers
		{
			class WeaponRefRemover
			{
				friend class FlowControl::Helpers::Resolver;

			public:
				WeaponRefRemover(state::Cards::Card & card) : card_(card) {}

			private:
				void operator()()
				{
					card_.ClearWeapon();
				}

			private:
				state::Cards::Card & card_;
			};
		}
	}
}