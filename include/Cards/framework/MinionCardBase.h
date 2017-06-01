#pragma once

#include "Cards/framework/GeneralCardBase.h"
#include "Cards/Database.h"
#include "state/Cards/Card.h"

namespace Cards
{
	template <typename T, typename... Ts>
		class MinionCardBase : public GeneralCardBase<T, Ts...>, protected MinionCardUtils
	{
	public:
		MinionCardBase() {
			Init(Cards::Database::GetInstance().Get(
				(Cards::CardId)CardClassIdMap<T>::id));
		}
		MinionCardBase(Database::CardData const& data) {
			Init(data);
		}

	private:
		void Init(Database::CardData const& data) {
			GeneralCardBase::Init(data);

			assert(data.card_type == state::kCardTypeMinion);

			BattlecryProcessor<T>::Process(*this);
		}

	protected:
		template <typename... Types>
		auto AdjacentBuffAura() { return AdjacentBuffHelper<T, Types...>(*this); }
	};
}