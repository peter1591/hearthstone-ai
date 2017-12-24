#pragma once

#include "state/Types.h"
#include "engine/FlowControl/FlowContext.h"
#include "engine/FlowControl/Manipulators/BoardManipulator.h"
#include "engine/FlowControl/Manipulators/CardManipulator.h"
#include "engine/FlowControl/Manipulators/CharacterManipulator.h"
#include "engine/FlowControl/Manipulators/OnBoardCharacterManipulator.h"
#include "engine/FlowControl/Manipulators/OnBoardMinionManipulator.h"
#include "engine/FlowControl/Manipulators/MinionManipulator.h"
#include "engine/FlowControl/Manipulators/HeroManipulator.h"
#include "engine/FlowControl/Manipulators/PlayerManipulator.h"
#include "engine/FlowControl/Manipulators/HeroPowerManipulator.h"
#include "engine/FlowControl/Manipulators/WeaponManipulator.h"
#include "engine/FlowControl/Manipulators/SecretManipulator.h"

namespace state {
	namespace aura {
		class Manager;
	}
}

namespace engine {
	namespace FlowControl
	{
		class Manipulate
		{
		public:
			Manipulate(state::State & state, FlowContext & flow_context);

			template <typename EventType, typename T>
			void AddEvent(T&& handler) const {
				return state_.AddEvent<EventType>(std::forward<T>(handler));
			}
			template <typename EventType, typename T>
			void AddEvent(state::CardRef card_ref, T&& handler) const {
				return state_.AddEvent<EventType>(card_ref, std::forward<T>(handler));
			}

			Manipulators::CardManipulator Card(state::CardRef ref) const;

			Manipulators::HeroManipulator CurrentHero() const;
			Manipulators::HeroManipulator OpponentHero() const;
			Manipulators::HeroManipulator Hero(state::PlayerIdentifier player) const;
			Manipulators::HeroManipulator Hero(state::CardRef hero_ref) const;

			Manipulators::PlayerManipulator Player(state::PlayerIdentifier player) const;
			Manipulators::PlayerManipulator CurrentPlayer() const;

			Manipulators::HeroPowerManipulator HeroPower(state::PlayerIdentifier player) const;
			Manipulators::HeroPowerManipulator HeroPower(state::CardRef hero_power_ref) const;

			Manipulators::OnBoardMinionManipulator OnBoardMinion(state::CardRef ref) const {
				return Manipulators::OnBoardMinionManipulator(state_, flow_context_, ref);
			}

			template <state::CardZone Zone>
			Manipulators::MinionManipulator<Zone> MinionInZone(state::CardRef ref) const {
				static_assert(Zone != state::kCardZonePlay, "Use OnBoardMinion() instead.");
				return Manipulators::MinionManipulator<Zone>(state_, flow_context_, ref);
			}

			Manipulators::OnBoardCharacterManipulator OnBoardCharacter(state::CardRef ref) const {
				return Manipulators::OnBoardCharacterManipulator(state_, flow_context_, ref);
			}

			Manipulators::WeaponManipulator Weapon(state::PlayerIdentifier player) const;
			Manipulators::WeaponManipulator Weapon(state::CardRef ref) const;

			Manipulators::SecretManipulator OnBoardSecret(state::CardRef ref) const;

			Manipulators::BoardManipulator Board() const;

		public: // bridge to flow context
			state::CardRef GetSpecifiedTarget() const { return flow_context_.GetSpecifiedTarget(); }
			Cards::CardId UserChooseOne(std::vector<Cards::CardId> const& cards) const { return flow_context_.UserChooseOne(cards); }
			IRandomGenerator & GetRandom() const { return flow_context_.GetRandom(); }

			void SaveUserChoice(Cards::CardId choice) const { flow_context_.SaveUserChoice(choice); }
			Cards::CardId GetSavedUserChoice() const { return flow_context_.GetSavedUserChoice(); }

		public: // bridge to state::State
			state::State const& GetState() const { return state_; }
			state::Cards::Card const& GetCard(state::CardRef ref) const;
			int GetCardAttackConsiderWeapon(state::CardRef ref) const;
			state::aura::Manager & Aura() const;

		public:
			state::CardRef GetRandomTarget(state::targetor::Targets const& target_info) const;

		private:
			state::State & state_;
			FlowControl::FlowContext & flow_context_;
		};
	}
}