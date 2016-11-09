#pragma once

#include "Entity/AuraAuxData.h"

namespace Manipulators
{
	namespace Helpers
	{
		class AuraHelper
		{
		public:
			AuraHelper(Entity::AuraAuxData& data) : data_(data) {}

		private:
			Entity::AuraAuxData & data_;
		};
	}
}