#pragma once

namespace neural_net {
	enum FieldSide {
		kCurrent,
		kOpponent,
		kInvalidSide
	};
	enum FieldType {
		kResourceCurrent,
		kResourceTotal,
		kResourceOverload,
		kResourceOverloadNext,
		kHeroHP,
		kHeroArmor,
		kMinionCount,
		kMinionHP,
		kMinionMaxHP,
		kMinionAttack,
		kMinionAttackable,
		kMinionTaunt,
		kMinionShield,
		kMinionStealth,
		kHandCount,
		kHandPlayable,
		kHandCost,
		kHeroPowerPlayable,
		kInvalidType
	};

	class IInputGetter {
	public:
		virtual ~IInputGetter() {}

		// for boolean value: 1 for true; 0 for false
		virtual float GetField(FieldSide field_side, FieldType field_type, int arg1 = 0) = 0;
	};
}