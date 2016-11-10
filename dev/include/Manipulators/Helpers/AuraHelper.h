#pragma once

#include "Entity/AuraAuxData.h"

class EntitiesManager;

namespace Manipulators
{
	namespace Helpers
	{
		class AuraHelper
		{
		public:
			AuraHelper(EntitiesManager& mgr, Entity::AuraAuxData& data);

			template <typename ClientAuraHelper>
			void Update(ClientAuraHelper&& client_aura_helper);

		private:
			EntitiesManager & mgr_;
			Entity::AuraAuxData & data_;
		};
	}
}