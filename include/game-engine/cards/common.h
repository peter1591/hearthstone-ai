#ifndef GAME_ENGINE_CARDS_COMMON_H
#define GAME_ENGINE_CARDS_COMMON_H

#include <type_traits>
#include "game-engine/board.h"
#include "game-engine/move.h"

#include "card-base.h"

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
	static bool AfterSummoned(int card_id, MinionIterator summoned_minion)
	{
		return CardCallbackManager::HandleCallback<Callback_AfterSummoned>(card_id, summoned_minion);
	}

	static bool GetRequiredTargets(int card_id, Player const& player, SlotIndexBitmap &targets, bool & meet_requirements)
	{
		return CardCallbackManager::HandleCallback<Callback_GetRequiredTargets>(card_id, player, targets, meet_requirements);
	}

	static bool BattleCry(int card_id, Player & player, Move::PlayMinionData const& play_minion_data)
	{
		return CardCallbackManager::HandleCallback<Callback_BattleCry>(card_id, player, play_minion_data);
	}

	static bool Weapon_AfterEquipped(int card_id, Hero & equipped_hero)
	{
		return CardCallbackManager::HandleCallback<Callback_Weapon_AfterEquipped>(card_id, equipped_hero);
	}

	static bool Weapon_BattleCry(int card_id, Player &equipping_player, SlotIndex target)
	{
		return CardCallbackManager::HandleCallback<Callback_Weapon_BattleCry>(card_id, equipping_player, target);
	}

	static bool Spell_Go(int card_id, Player &player, SlotIndex target)
	{
		return CardCallbackManager::HandleCallback<Callback_Spell_Go>(card_id, player, target);
	}

private:
	template <typename Card, typename Callback> struct CardCallbackCaller {};

	DECLARE_CARD_CALLBACK(AfterSummoned)
	DECLARE_CARD_CALLBACK(BattleCry)
	DECLARE_CARD_CALLBACK(GetRequiredTargets)
	DECLARE_CARD_CALLBACK(Weapon_BattleCry)
	DECLARE_CARD_CALLBACK(Weapon_AfterEquipped)
	DECLARE_CARD_CALLBACK(Spell_Go)

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

HANDLE_CARD_CALLBACK(Card_CS2_023)
HANDLE_CARD_CALLBACK(Card_CS2_025)
HANDLE_CARD_CALLBACK(Card_CS2_029)
HANDLE_CARD_CALLBACK(Card_CS2_188)
HANDLE_CARD_CALLBACK(Card_CS2_189)
HANDLE_CARD_CALLBACK(Card_CS2_222)
HANDLE_CARD_CALLBACK(Card_EX1_029)
HANDLE_CARD_CALLBACK(Card_EX1_277)
HANDLE_CARD_CALLBACK(Card_EX1_390)
HANDLE_CARD_CALLBACK(Card_EX1_412)
HANDLE_CARD_CALLBACK(Card_EX1_584)
HANDLE_CARD_CALLBACK(Card_EX1_593)
HANDLE_CARD_CALLBACK(Card_FP1_002)
HANDLE_CARD_CALLBACK(Card_FP1_007)
HANDLE_CARD_CALLBACK(Card_FP1_021)
HANDLE_CARD_CALLBACK(Card_GVG_043)
HANDLE_CARD_CALLBACK(Card_GVG_051)
HANDLE_CARD_CALLBACK(Card_GVG_059)
HANDLE_CARD_CALLBACK(Card_GVG_112)
HANDLE_CARD_CALLBACK(Card_BRM_019)
HANDLE_CARD_CALLBACK(Card_NEW1_018)
HANDLE_CARD_CALLBACK(Card_LOE_051)
HANDLE_CARD_CALLBACK(Card_AT_117)

#undef HANDLE_CARD_CALLBACK
	default:
		return false; // no any special mechanism for this card
	}
}

}; // namespace Cards
}; // namespace GameEngine

#endif
