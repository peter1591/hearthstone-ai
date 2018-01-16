#pragma once

#include <memory>
#include "state/State.h"

namespace neural_net {
	namespace impl {
		class NeuralNetworkWrapperImpl;
		class NeuralNetworkInputDataWrapperImpl;
		class NeuralNetworkOutputDataWrapperImpl;
	}

	enum class FieldSide {
		kCurrent,
		kOpponent,
		kInvalidSide
	};
	enum class FieldType {
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

	class NeuralNetworkInputDataWrapper
	{
		friend class NeuralNetworkWrapper;

	public:
		NeuralNetworkInputDataWrapper();
		~NeuralNetworkInputDataWrapper();

		void AddData(IInputGetter * getter);

	private:
		impl::NeuralNetworkInputDataWrapperImpl * impl_;
	};

	class NeuralNetworkOutputDataWrapper
	{
		friend class NeuralNetworkWrapper;

	public:
		NeuralNetworkOutputDataWrapper();
		~NeuralNetworkOutputDataWrapper();

		void AddData(int label);

	private:
		impl::NeuralNetworkOutputDataWrapperImpl * impl_;
	};

	class NeuralNetworkWrapper
	{
	public:
		NeuralNetworkWrapper();
		~NeuralNetworkWrapper();

		NeuralNetworkWrapper(NeuralNetworkWrapper const& rhs) = delete;
		NeuralNetworkWrapper & operator=(NeuralNetworkWrapper const& rhs) = delete;

	public:
		void InitializeModel(std::string const& path);
		void LoadModel(std::string const& path);

		void Train(
			NeuralNetworkInputDataWrapper const& input,
			NeuralNetworkOutputDataWrapper const& output,
			size_t batch_size, int epoch);

		void Save(std::string const& path);

		// @return tuple of (correct, total)
		std::pair<uint64_t, uint64_t> Verify(
			NeuralNetworkInputDataWrapper const& input,
			NeuralNetworkOutputDataWrapper const& output);

		void Predict(impl::NeuralNetworkInputDataWrapperImpl const& input, std::vector<double> & results);

	private:
		impl::NeuralNetworkWrapperImpl * impl_;
	};
}