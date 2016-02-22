#ifndef GAME_ENGINE_CARDS_COMMON_H
#define GAME_ENGINE_CARDS_COMMON_H

#include <type_traits>
#include "game-engine/board.h"
#include "game-engine/move.h"

#include "card_CS2_188.h"
#include "card_CS2_189.h"
#include "card_CS2_222.h"
#include "card_EX1_029.h"
#include "card_EX1_390.h"
#include "card_EX1_412.h"
#include "card_FP1_007.h"
#include "card_FP1_002.h"
#include "card_GVG_051.h"
#include "card_GVG_059.h"
#include "card_BRM_019.h"
#include "card_NEW1_018.h"

namespace GameEngine {
	namespace Cards {

#define DECLARE_CARD_CALLBACK(MethodName) \
	struct Callback_ ## MethodName {}; \
	template <typename Card> \
	struct CardCallbackCaller<Card, Callback_ ## MethodName> \
	{ \
	private: \
		typedef char test_ret_if_yes; \
		typedef long test_ret_if_no; \
		template <class Card, typename T = decltype(&Card:: ## MethodName)> \
		struct TestResultWrapper { \
			typedef test_ret_if_yes type; \
		}; \
		template <typename T> static typename TestResultWrapper<T>::type TestMethodExists(void*); \
			template <typename T> static test_ret_if_no TestMethodExists(...); \
			template <typename TestResult, typename... Params> \
			static bool CallInternal(std::enable_if_t<(sizeof(TestResult) == sizeof(test_ret_if_yes))>*, Params&&... params) \
		{ \
			Card:: ## MethodName(params...); \
			return true; \
		} \
		template <typename TestResult, typename... Params> \
		static bool CallInternal(std::enable_if_t<(sizeof(TestResult) != sizeof(test_ret_if_yes))>*, Params&&...) \
		{ \
			return false; \
		} \
	public: \
		template <typename... Params> static bool Call(Params&&... params) \
		{ \
			typedef decltype(TestMethodExists<Card>(nullptr)) TestResult; \
			return CallInternal<TestResult, Params...>(nullptr, params...); \
		} \
	};

class CardCallbackManager
{
public:
	static bool AfterSummoned(int card_id, GameEngine::BoardObjects::Minion & summoned_minion)
	{
		return CardCallbackManager::HandleCallback<Callback_AfterSummoned>(card_id, summoned_minion);
	}

	static bool GetRequiredTargets(int card_id, GameEngine::Board const& board, SlotIndex side, SlotIndexBitmap &targets, bool & meet_requirements)
	{
		return CardCallbackManager::HandleCallback<Callback_GetRequiredTargets>(card_id, board, side, targets, meet_requirements);
	}

	static bool BattleCry(int card_id, GameEngine::Board &board, SlotIndex playing_side, GameEngine::Move::PlayMinionData const& play_minion_data)
	{
		return CardCallbackManager::HandleCallback<Callback_BattleCry>(card_id, board, playing_side, play_minion_data);
	}

	static bool Weapon_AfterEquipped(int card_id, GameEngine::BoardObjects::Hero & equipped_hero)
	{
		return CardCallbackManager::HandleCallback<Callback_Weapon_AfterEquipped>(card_id, equipped_hero);
	}

	static bool Weapon_BattleCry(int card_id, GameEngine::Board &board, SlotIndex playing_side, GameEngine::Move::EquipWeaponData const& equip_weapon_data)
	{
		return CardCallbackManager::HandleCallback<Callback_Weapon_BattleCry>(card_id, board, playing_side, equip_weapon_data);
	}

private:
	template <typename Card, typename Callback> struct CardCallbackCaller {};

	DECLARE_CARD_CALLBACK(AfterSummoned)
	DECLARE_CARD_CALLBACK(BattleCry)
	DECLARE_CARD_CALLBACK(GetRequiredTargets)
	DECLARE_CARD_CALLBACK(Weapon_BattleCry)
	DECLARE_CARD_CALLBACK(Weapon_AfterEquipped)

#undef DECLARE_CARD_CALLBACK

private:
	template <typename Callback, typename... Params>
	static bool HandleCallback(int card_id, Params&&... params);
};

template <typename Callback, typename ...Params>
inline bool CardCallbackManager::HandleCallback(int card_id, Params&&... params)
{
	switch (card_id)
	{
#define HANDLE_CARD_CALLBACK(CardClassName) \
	case CardClassName::card_id: \
		return CardCallbackCaller<CardClassName, Callback>::Call(params...);

HANDLE_CARD_CALLBACK(Card_CS2_188)
HANDLE_CARD_CALLBACK(Card_CS2_189)
HANDLE_CARD_CALLBACK(Card_CS2_222)
HANDLE_CARD_CALLBACK(Card_EX1_029)
HANDLE_CARD_CALLBACK(Card_EX1_390)
HANDLE_CARD_CALLBACK(Card_EX1_412)
HANDLE_CARD_CALLBACK(Card_FP1_007)
HANDLE_CARD_CALLBACK(Card_FP1_002)
HANDLE_CARD_CALLBACK(Card_GVG_051)
HANDLE_CARD_CALLBACK(Card_GVG_059)
HANDLE_CARD_CALLBACK(Card_BRM_019)
HANDLE_CARD_CALLBACK(Card_NEW1_018)

#undef HANDLE_CARD_CALLBACK
	default:
		return false; // no any special mechanism for this card
	}
}

}; // namespace Cards
}; // namespace GameEngine

#endif
