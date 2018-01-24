#pragma once

#include <memory>
#include "state/State.h"

namespace neural_net {
	namespace impl {
		class NeuralNetworkImpl;
		class NeuralNetworkInputImpl;
		class NeuralNetworkOutputImpl;
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
		virtual double GetField(FieldSide field_side, FieldType field_type, int arg1 = 0) const = 0;
	};

	class NeuralNetworkInput
	{
		friend class NeuralNetwork;

	public:
		NeuralNetworkInput();
		~NeuralNetworkInput();

		NeuralNetworkInput(NeuralNetworkInput const&) = delete;
		NeuralNetworkInput & operator=(NeuralNetworkInput const&) = delete;

		void AddData(IInputGetter const* getter);
		void Clear();

	private:
		impl::NeuralNetworkInputImpl * impl_;
	};

	class NeuralNetworkOutput
	{
		friend class NeuralNetwork;

	public:
		NeuralNetworkOutput();
		~NeuralNetworkOutput();

		NeuralNetworkOutput(NeuralNetworkOutput const&) = delete;
		NeuralNetworkOutput & operator=(NeuralNetworkOutput const&) = delete;

		void AddData(int label);
		void Clear();

	private:
		impl::NeuralNetworkOutputImpl * impl_;
	};

	class NeuralNetwork
	{
	public:
		NeuralNetwork();
		~NeuralNetwork();

		NeuralNetwork(NeuralNetwork const&) = delete;
		NeuralNetwork & operator=(NeuralNetwork const&) = delete;

		NeuralNetwork(NeuralNetwork &&);
		NeuralNetwork & operator=(NeuralNetwork &&);

	public:
		static void CreateWithRandomWeights(std::string const& path);

		void Save(std::string const& path) const;
		void Load(std::string const& path, bool is_random = false);

		void CopyFrom(NeuralNetwork const& rhs);

		void Train(
			NeuralNetworkInput const& input,
			NeuralNetworkOutput const& output,
			size_t batch_size, int epoch);

		// @return tuple of (correct, total)
		std::pair<uint64_t, uint64_t> Verify(
			NeuralNetworkInput const& input,
			NeuralNetworkOutput const& output);

		double Predict(IInputGetter * input, std::mt19937 & random);
		void Predict(impl::NeuralNetworkInputImpl const& input, std::vector<double> & results, std::mt19937 & random);

	private:
		impl::NeuralNetworkImpl * impl_;
	};
}