#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4083 4244 4267)
#endif

#define CNN_SINGLE_THREAD

#ifdef _MSC_VER
#define CNN_USE_SSE
#include <intrin.h> // workaround tiny_dnn issue
#endif

#include <chrono> // workaround tiny_dnn issue #872
#include "tiny_dnn/tiny_dnn.h"

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#include "neural_net/NeuralNetwork.h"

namespace neural_net {
	namespace impl {
		class NeuralNetworkOutputDataWrapperImpl
		{
		public:
			void AddData(int label) {
				tiny_dnn::vec_t output;
				output.push_back((float)label);
				output_.push_back(output);
			}
			
			auto const& GetData() const { return output_; }

		private:
			std::vector<tiny_dnn::vec_t> output_;
		};

		class NeuralNetworkInputDataWrapperImpl
		{
		public:
			void AddData(IInputGetter * getter) {
				tiny_dnn::tensor_t input;
				GetInputData(getter, input);
				input_.push_back(input);
			}

			auto const& GetData() const { return input_; }

		private:
			void GetInputData(IInputGetter * getter, tiny_dnn::tensor_t & data) {
				tiny_dnn::vec_t input1;
				AddHeroData(FieldSide::kCurrent, getter, input1);
				AddHeroData(FieldSide::kOpponent, getter, input1);
				data.push_back(std::move(input1));

				tiny_dnn::vec_t input2;
				AddMinionsData(FieldSide::kCurrent, getter, input2);
				AddMinionsData(FieldSide::kOpponent, getter, input2);
				data.push_back(std::move(input2));

				tiny_dnn::vec_t input3;
				AddStandAloneData(getter, input3);
				data.push_back(std::move(input3));
			}

			void AddHeroData(
				FieldSide side,
				IInputGetter * getter,
				tiny_dnn::vec_t & data)
			{
				double hp = getter->GetField(side, FieldType::kHeroHP) +
					getter->GetField(side, FieldType::kHeroArmor);
				data.push_back(NormalizeFromUniformDist(hp, 0.0, 30.0));
				//data.push_back(player["attack"].asFloat());
				//data.push_back(player["attackable"].asBool());
			}

			void AddMinionsData(
				FieldSide side,
				IInputGetter * getter,
				tiny_dnn::vec_t & data)
			{
				int rest = 7;
				for (int i = 0; i < (int)getter->GetField(side, FieldType::kMinionCount); ++i) {
					if (rest <= 0) throw std::runtime_error("too many minions");
					AddMinionData(side, getter, i, data);
					--rest;
				}
				while (rest > 0) {
					AddMinionPlaceHolderData(data);
					--rest;
				}
			}

			void AddMinionData(
				FieldSide side,
				IInputGetter * getter,
				int minion_idx,
				tiny_dnn::vec_t & data)
			{
				data.push_back(NormalizeFromUniformDist(getter->GetField(side, FieldType::kMinionHP, minion_idx), 1.0, 7.0));
				data.push_back(NormalizeFromUniformDist(getter->GetField(side, FieldType::kMinionMaxHP, minion_idx), 1.0, 7.0));
				data.push_back(NormalizeFromUniformDist(getter->GetField(side, FieldType::kMinionAttack, minion_idx), 0.0, 7.0));
				data.push_back(NormalizeBool(getter->GetField(side, FieldType::kMinionAttackable, minion_idx)));
				data.push_back(NormalizeBool(getter->GetField(side, FieldType::kMinionTaunt, minion_idx)));
				data.push_back(NormalizeBool(getter->GetField(side, FieldType::kMinionShield, minion_idx)));
				data.push_back(NormalizeBool(getter->GetField(side, FieldType::kMinionStealth, minion_idx)));
			}

			void AddMinionPlaceHolderData(tiny_dnn::vec_t & data) {
				data.push_back(0.0);
				data.push_back(0.0);
				data.push_back(0.0);
				data.push_back(NormalizeBool(false));
				data.push_back(NormalizeBool(false));
				data.push_back(NormalizeBool(false));
				data.push_back(NormalizeBool(false));
			}

			void AddStandAloneData(
				IInputGetter * getter,
				tiny_dnn::vec_t & data)
			{
				data.push_back(NormalizeFromUniformDist(getter->GetField(
					FieldSide::kCurrent, FieldType::kResourceCurrent), 0, 10));
				data.push_back(NormalizeFromUniformDist(getter->GetField(
					FieldSide::kCurrent, FieldType::kResourceTotal), 0, 10));
				data.push_back(NormalizeFromUniformDist(getter->GetField(
					FieldSide::kCurrent, FieldType::kResourceOverloadNext), 0, 10));

				int cur_hand_count = (int)getter->GetField(FieldSide::kCurrent, FieldType::kHandCount);
				data.push_back(NormalizeFromUniformDist(cur_hand_count, 0, 10));

				int cur_hand_playable = 0;
				for (int i = 0; i < cur_hand_count; ++i) {
					bool playable = getter->GetField(FieldSide::kCurrent, FieldType::kHandPlayable, i);
					if (playable) {
						++cur_hand_playable;
					}
				}
				data.push_back(NormalizeFromUniformDist(cur_hand_playable, 0, 10));

				int hand_cards = 0;
				for (int i = 0; i < cur_hand_count; ++i) {
					data.push_back(NormalizeFromUniformDist(
						getter->GetField(FieldSide::kCurrent, FieldType::kHandCost, i), 0, 10));
					++hand_cards;
				}
				while (hand_cards < 10) {
					data.push_back(NormalizeFromUniformDist(-1, 0, 10));
					++hand_cards;
				}

				int opn_hand_count = (int)getter->GetField(FieldSide::kOpponent, FieldType::kHandCount);
				data.push_back(NormalizeFromUniformDist(opn_hand_count, 0, 10));

				data.push_back(NormalizeBool(
					getter->GetField(FieldSide::kCurrent, FieldType::kHeroPowerPlayable)));
			}

			tiny_dnn::float_t NormalizeFromUniformDist(double v, double min, double max) {
				// normalize to mean = 0, var = 1.0
				// uniform dist is with variance = (max-min)^2 / 12
				// --> so we should have (max-min)^2 / 12 = 1.0
				// --> (max-min)^2 = 12.0
				// --> (max-min) = sqrt(12.0)
				static double sqrt_12 = std::sqrt(12.0);

				double mean = (min + max) / 2;
				double range = (max - min);
				double scale = sqrt_12 / range;

				double ret = (v - mean) * scale;
				return (tiny_dnn::float_t)ret;
			}

			tiny_dnn::float_t NormalizeBool(bool v) {
				double vv = 0.0;
				if (v) vv = 1.0;
				else vv = -1.0;
				double ret = NormalizeFromUniformDist(vv, -1.0, 1.0);
				return (tiny_dnn::float_t)ret;
			}

		private:
			std::vector<tiny_dnn::tensor_t> input_;
		};
		
		class NeuralNetworkWrapperImpl
		{
		public:
			static void InitializeModel(std::string const& filename) {
				static constexpr int hero_in_dim = 1;
				static constexpr int hero_out_dim = 1;

				static constexpr int minion_in_dim = 7;
				static constexpr int minion_out_dim = 3;

				static constexpr int minion_count = 7;

				static constexpr int standalone_in_dim = 17;

				tiny_dnn::layers::input in_heroes(tiny_dnn::shape3d(
					hero_in_dim,
					2,  // current hero, opponent hero
					1));
				tiny_dnn::layers::conv hero_conv1(
					hero_in_dim, 2,
					hero_in_dim, 1,
					1, hero_out_dim);
				in_heroes << hero_conv1; // @out: 1 * 2 * hero_out_dim
				auto hero_conv1a = tiny_dnn::activation::leaky_relu();
				hero_conv1 << hero_conv1a; // @out: 1 * 2 * hero_out_dim

				tiny_dnn::layers::input in_minions(tiny_dnn::shape3d(
					minion_in_dim,
					minion_count * 2, // current minions, opponent minions
					1));
				tiny_dnn::layers::conv minion_conv1(
					minion_in_dim, minion_count * 2,
					minion_in_dim, 1,
					1, 1 * minion_out_dim);
				in_minions << minion_conv1; // @out: 1 * (minion_count * 2) * minion_out_dim)
				auto minion_conv1a = tiny_dnn::activation::leaky_relu();
				minion_conv1 << minion_conv1a; // @out: 1 * (minion_count * 2) * minion_out_dim)

				tiny_dnn::layers::input in_standalone(tiny_dnn::shape3d(
					1, 1, standalone_in_dim));

				auto concat = tiny_dnn::layers::concat({
					tiny_dnn::shape3d(1,1,2 * hero_out_dim), // reshape to 1x1
					tiny_dnn::shape3d(1,1,minion_count * 2 * minion_out_dim), // reshape to 1x1
					in_standalone.out_shape()[0] });
				(hero_conv1a, minion_conv1a, in_standalone) << concat;

				auto fc1 = tiny_dnn::fully_connected_layer(
					2 * hero_out_dim + 2 * minion_count * minion_out_dim + standalone_in_dim,
					10);
				concat << fc1;

				auto fc1a = tiny_dnn::activation::leaky_relu();
				auto fc2 = tiny_dnn::fully_connected_layer(10, 1);
				fc1 << fc1a << fc2;

				tiny_dnn::network<tiny_dnn::graph> net;
				tiny_dnn::construct_graph(net, { &in_heroes, &in_minions, &in_standalone }, { &fc2 });

				net.init_weight();

				net.save(filename);
			}

			void LoadModel(std::string const& filename) {
				net_.load(filename);
			}

			void Train(
				impl::NeuralNetworkInputDataWrapperImpl const& input,
				impl::NeuralNetworkOutputDataWrapperImpl const& output,
				size_t batch_size, int epoch) 
			{
				tiny_dnn::adam opt;
				net_.fit<tiny_dnn::mse>(opt, input.GetData(), output.GetData(), batch_size, epoch, []() {}, []() {});
			}

			void Save(std::string const& name) {
				net_.save(name);
			}

			std::pair<uint64_t, uint64_t> Verify(
				impl::NeuralNetworkInputDataWrapperImpl const& input,
				impl::NeuralNetworkOutputDataWrapperImpl const& output)
			{
				uint64_t correct = 0;
				uint64_t total = 0;

				auto const& input_data = input.GetData();
				auto const& output_data = output.GetData();

				total = input_data.size();
				for (size_t idx = 0; idx < input_data.size(); ++idx) {
					auto result = net_.predict(input_data[idx]);
					bool predict_win = (result[0][0] > 0.0); // TODO: this logic should move to caller
					bool actual_win = (output_data[idx][0] > 0.0);
					if (predict_win == actual_win) ++correct;
				}

				return { correct, total };
			}

			void Predict(impl::NeuralNetworkInputDataWrapperImpl const& input, std::vector<double> & results) {
				auto const& input_data = input.GetData();
				results.clear();
				results.reserve(input_data.size());
				for (size_t idx = 0; idx < input_data.size(); ++idx) {
					results.push_back(net_.predict(input_data[0])[0][0]);
				}
			}

		private:
			tiny_dnn::network<tiny_dnn::graph> net_;
		};
	}

	NeuralNetworkInputDataWrapper::NeuralNetworkInputDataWrapper() {
		impl_ = new impl::NeuralNetworkInputDataWrapperImpl();
	}
	NeuralNetworkInputDataWrapper::~NeuralNetworkInputDataWrapper() {
		delete impl_;
	}
	void NeuralNetworkInputDataWrapper::AddData(IInputGetter * getter)
	{
		impl_->AddData(getter);
	}

	NeuralNetworkOutputDataWrapper::NeuralNetworkOutputDataWrapper() {
		impl_ = new impl::NeuralNetworkOutputDataWrapperImpl();
	}
	NeuralNetworkOutputDataWrapper::~NeuralNetworkOutputDataWrapper() {
		delete impl_;
	}
	void NeuralNetworkOutputDataWrapper::AddData(int label)
	{
		impl_->AddData(label);
	}


	NeuralNetworkWrapper::NeuralNetworkWrapper() :
		impl_(new impl::NeuralNetworkWrapperImpl())
	{
	}

	NeuralNetworkWrapper::~NeuralNetworkWrapper()
	{
		delete impl_;
	}

	void NeuralNetworkWrapper::InitializeModel(std::string const& path) { impl_->InitializeModel(path); }
	void NeuralNetworkWrapper::LoadModel(std::string const& path) { impl_->LoadModel(path); }

	void NeuralNetworkWrapper::Train(
		NeuralNetworkInputDataWrapper const& input,
		NeuralNetworkOutputDataWrapper const& output,
		size_t batch_size, int epoch)
	{
		impl_->Train(*input.impl_, *output.impl_, batch_size, epoch);
	}
	
	void NeuralNetworkWrapper::Save(std::string const& path) { return impl_->Save(path); }

	// @return tuple of (correct, total)
	std::pair<uint64_t, uint64_t> NeuralNetworkWrapper::Verify(
		NeuralNetworkInputDataWrapper const& input,
		NeuralNetworkOutputDataWrapper const& output)
	{
		return impl_->Verify(*input.impl_, *output.impl_);
	}

	void NeuralNetworkWrapper::Predict(impl::NeuralNetworkInputDataWrapperImpl const& input, std::vector<double> & results)
	{
		return impl_->Predict(input, results);
	}
}
