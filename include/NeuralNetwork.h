#pragma once

#include <memory>
#include "state/State.h"

namespace impl {
	class NeuralNetworkWrapperImpl;
}

class NeuralNetworkWrapper
{
public:
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
		virtual double GetField(FieldSide field_side, FieldType field_type, int arg1 = 0) = 0;
	};

	NeuralNetworkWrapper() : impl_(nullptr) {}
	~NeuralNetworkWrapper();

	NeuralNetworkWrapper(NeuralNetworkWrapper const& rhs) = delete;
	NeuralNetworkWrapper & operator=(NeuralNetworkWrapper const& rhs) = delete;

public: // for training
	void InitializeTrain();
	void AddTrainData(IInputGetter * getter, int label, bool for_validate);
	void Train();

public: // for prediction
	void InitializePredict(std::string const& filename);
	double Predict(IInputGetter * getter);

private:
	impl::NeuralNetworkWrapperImpl * impl_;
};