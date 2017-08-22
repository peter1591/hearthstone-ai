#pragma once

#include <string>

#include <json/json.h>

#include "state/State.h"

namespace state
{
	class JsonSerializer
	{
	public:
		static Json::Value Serialize(State const& state) {
			Json::Value obj;
			obj["current_player"] = GetPlayerString(state.GetCurrentPlayerId());
			obj["turn"] = state.GetTurn();
			obj["first_player"] = Serialize(state, state.GetBoard().GetFirst());
			obj["second_player"] = Serialize(state, state.GetBoard().GetSecond());
			return obj;
		}

	private:
		static Json::Value Serialize(State const& state, board::Player const& player) {
			Json::Value obj;
			obj["hero"] = SerializeHero(state, player.GetHeroRef());
			obj["hero_power"] = SerializeHeroPower(state, player.GetHeroPowerRef());
			obj["weapon"] = SerializeWeapon(state, player.GetWeaponRef());
			obj["resource"] = Serialize(state, player.GetResource());
			obj["fatigue"] = player.GetFatigueDamage();

			obj["deck"] = Serialize(state, player.deck_);
			obj["hand"] = Serialize(state, player.hand_);
			obj["minions"] = Serialize(state, player.minions_);
			obj["secrets"] = Serialize(state, player.secrets_);

			return obj;
		}

		static Json::Value Serialize(State const& state, board::PlayerResource const& resource) {
			Json::Value obj;
			obj["current"] = resource.GetCurrent();
			obj["total"] = resource.GetTotal();
			obj["overload_current"] = resource.GetCurrentOverloaded();
			obj["overload_next"] = resource.GetNextOverload();
			return obj;
		}

		static Json::Value Serialize(State const& state, board::Deck const& deck) {
			Json::Value obj;
			obj["count"] = deck.Size();
			return obj;
		}

		static Json::Value Serialize(State const& state, board::Hand const& hand) {
			Json::Value obj;
			hand.ForEach([&](CardRef ref) {
				obj.append(SerializeHandCard(state, ref));
				return true;
			});
			return obj;
		}

		static Json::Value Serialize(State const& state, board::Minions const& minions) {
			Json::Value obj;
			minions.ForEach([&](CardRef ref) {
				obj.append(SerializeMinion(state, ref));
				return true;
			});
			return obj;
		}

		static Json::Value Serialize(State const& state, board::Secrets const& secrets) {
			Json::Value obj;
			secrets.ForEach([&](CardRef ref) {
				obj.append(SerializeSecret(state, ref));
				return true;
			});
			return obj;
		}

		static Json::Value SerializeHero(State const& state, CardRef ref) {
			Cards::Card const& card = state.GetCard(ref);
			Json::Value obj;
			obj["card_id"] = (int)card.GetCardId();
			obj["hp"] = card.GetHP();
			obj["max_hp"] = card.GetMaxHP();
			obj["armor"] = card.GetArmor();
			obj["attack"] = card.GetAttack();
			return obj;
		}
		
		static Json::Value SerializeHeroPower(State const& state, CardRef ref) {
			Cards::Card const& card = state.GetCard(ref);
			Json::Value obj;
			obj["card_id"] = (int)card.GetCardId();
			obj["usable"] = card.GetRawData().usable;
			return obj;
		}

		static Json::Value SerializeWeapon(State const& state, CardRef ref) {
			Json::Value obj;
			if (ref.IsValid()) {
				Cards::Card const& card = state.GetCard(ref);
				obj["card_id"] = (int)card.GetCardId();
				obj["attack"] = card.GetAttack();
				obj["hp"] = card.GetHP();
			}
			return obj;
		}

		static Json::Value SerializeHandCard(State const& state, CardRef ref) {
			Cards::Card const& card = state.GetCard(ref);
			Json::Value obj;
			obj["card_id"] = (int)card.GetCardId();
			obj["cost"] = card.GetCost();
			return obj;
		}

		static Json::Value SerializeMinion(State const& state, CardRef ref) {
			Cards::Card const& card = state.GetCard(ref);
			Json::Value obj;
			obj["card_id"] = (int)card.GetCardId();
			obj["cost"] = card.GetCost();
			obj["hp"] = card.GetHP();
			obj["max_hp"] = card.GetMaxHP();
			obj["attack"] = card.GetAttack();
			obj["taunt"] = card.HasTaunt();
			obj["shield"] = card.HasShield();
			obj["stealth"] = card.HasStealth();
			return obj;
		}

		static Json::Value SerializeSecret(State const& state, CardRef ref) {
			Cards::Card const& card = state.GetCard(ref);
			Json::Value obj;
			obj["card_id"] = (int)card.GetCardId();
			return obj;
		}

	private:
		static std::string GetPlayerString(state::PlayerIdentifier player) {
			if (player.IsFirst()) return "kFirstPlayer";
			if (player.IsSecond()) return "kSecondPlayer";
			throw std::runtime_error("Unknown player");
		}
	};
}