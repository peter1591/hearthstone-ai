#pragma once

#include <utility>

#include "Utils/CloneableContainers/Vector.h"
#include "State/Cards/Card.h"
#include "State/Types.h"

#include "FlowControl/Manipulate.h"

namespace FlowControl
{
	class FlowContext;
}

namespace state
{
	class State;

	namespace Cards
	{
		class Manager
		{
		public:
			typedef Utils::CloneableContainers::Vector<Card> ContainerType;

			const Card & Get(const CardRef & id) const
			{
				return cards_.Get(id.id);
			}

			template <typename T> CardRef PushBack(State & state, FlowControl::FlowContext & flow_context, T&& card);

		public: // manipulators
			FlowControl::Manipulators::CardManipulator GetCardManipulator(State& state, FlowControl::FlowContext& flow_context, CardRef ref) {
				return FlowControl::Manipulators::CardManipulator(state, flow_context, ref, GetMutable(ref));
			}

			FlowControl::Manipulators::HeroManipulator GetHeroManipulator(State& state, FlowControl::FlowContext& flow_context, CardRef ref, PlayerIdentifier player) {
				return FlowControl::Manipulators::HeroManipulator(state, flow_context, ref, GetMutable(ref), player);
			}

			FlowControl::Manipulators::MinionManipulator GetMinionManipulator(State& state, FlowControl::FlowContext& flow_context, CardRef ref) {
				return FlowControl::Manipulators::MinionManipulator(state, flow_context, ref, GetMutable(ref));
			}

			FlowControl::Manipulators::CharacterManipulator GetCharacterManipulator(State& state, FlowControl::FlowContext& flow_context, CardRef ref) {
				return FlowControl::Manipulators::CharacterManipulator(state, flow_context, ref, GetMutable(ref));
			}

			FlowControl::Manipulators::WeaponManipulator GetWeaponManipulator(State& state, FlowControl::FlowContext& flow_context, CardRef ref) {
				return FlowControl::Manipulators::WeaponManipulator(state, flow_context, ref, GetMutable(ref));
			}

		private:
			Card & GetMutable(const CardRef & id)
			{
				return cards_.Get(id.id);
			}

		private:
			ContainerType cards_;
		};
	}
}