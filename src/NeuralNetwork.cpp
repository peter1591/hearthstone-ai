#pragma warning (push)
#pragma warning (disable: 4083 4244 4267)
#define CNN_SINGLE_THREAD

#ifdef _MSC_VER
#define CNN_USE_SSE
#include <intrin.h> // workaround tiny_dnn issue
#endif

#include <chrono> // workaround tiny_dnn issue #872
#include "tiny_dnn/tiny_dnn.h"
#pragma warning (pop)

#include "NeuralNetwork.h"

namespace impl {
	class NeuralNetworkWrapperImpl
	{
	public:
		void InitializeTrain() {
		}

		void AddTrainData(NeuralNetworkWrapper::IInputGetter * getter, int label, bool for_validate) {
			tiny_dnn::tensor_t input;
			GetInputData(getter, input);

			tiny_dnn::vec_t output;
			output.push_back((float)label);

			if (for_validate) {
				validate_input_.push_back(input);
				validate_output_.push_back(output);
			}
			else {
				input_.push_back(input);
				output_.push_back(output);
			}
		}

		void Train() {
			static constexpr int hero_in_dim = 1;
			static constexpr int hero_out_dim = 1;

			static constexpr int minion_in_dim = 7;
			static constexpr int minion_out_dim = 3;

			static constexpr int minion_count = 7;

			static constexpr int standalone_in_dim = 17;

			tiny_dnn::layers::input in_heroes(tiny_dnn::shape3d(
				hero_in_dim, 1,
				2)); // current hero, opponent hero
			tiny_dnn::layers::conv hero_conv1(
				hero_in_dim, 1,
				hero_in_dim, 1,
				2, 2 * hero_out_dim);
			in_heroes << hero_conv1; // @out: 1 * 1 * (2*hero_out_dim)
			auto hero_conv1a = tiny_dnn::activation::leaky_relu();
			hero_conv1 << hero_conv1a; // @out: 1 * 1 * (2 * hero_out_dim)

			tiny_dnn::layers::input in_minions(tiny_dnn::shape3d(
				minion_in_dim, 1,
				minion_count * 2)); // current minions, opponent minions
			tiny_dnn::layers::conv minion_conv1(
				minion_in_dim, 1,
				minion_in_dim, 1,
				minion_count * 2, minion_count * 2 * minion_out_dim);
			in_minions << minion_conv1; // @out: 1 * 1 * (minion_count * 2 * minion_out_dim)
			auto minion_conv1a = tiny_dnn::activation::leaky_relu();
			minion_conv1 << minion_conv1a; // @out: 1 * 1 * (minion_count * 2 * minion_out_dim)

			tiny_dnn::layers::input in_standalone(tiny_dnn::shape3d(
				1, 1, standalone_in_dim));

			auto concat = tiny_dnn::layers::concat({
				hero_conv1a.out_shape()[0],
				tiny_dnn::shape3d(1,1,minion_count * 2 * minion_out_dim),
				in_standalone.out_shape()[0] });
			(hero_conv1a, minion_conv1a, in_standalone) << concat;

			auto fc1 = tiny_dnn::fully_connected_layer(
				2 * hero_out_dim + 2 * minion_count * minion_out_dim + standalone_in_dim,
				10);
			concat << fc1;

			auto fc1a = tiny_dnn::activation::leaky_relu();
			auto fc2 = tiny_dnn::fully_connected_layer(10, 1);
			fc1 << fc1a << fc2;

			tiny_dnn::construct_graph(net_, { &in_heroes, &in_minions, &in_standalone }, { &fc2 });

			size_t batch_size = 32;
			int epoch = 1;
			size_t total_epoch = 0;
			tiny_dnn::adam opt;

			while (true) {
				int batch_op_counter = 0;
				auto batch_op = [batch_op_counter]() mutable {
					++batch_op_counter;
					if (batch_op_counter % 100 == 0) {
						std::cout << "completed batches: " << batch_op_counter << std::endl;
					}
				};

				auto epoch_op = [&total_epoch]() mutable {
					++total_epoch;
					std::cout << "completed epoch: " << total_epoch << std::endl;
				};
				net_.fit<tiny_dnn::mse>(opt, input_, output_, batch_size, epoch, batch_op, epoch_op);

				std::stringstream ss;
				ss << "net_result_epoch_" << total_epoch;
				net_.save(ss.str());

				size_t correct = 0;

				for (size_t idx = 0; idx < input_.size(); ++idx) {
					auto result = net_.predict(input_[idx]);
					bool predict_win = (result[0][0] > 0.0);
					bool actual_win = (output_[idx][0] > 0.0);
					if (predict_win == actual_win) ++correct;
				}
				double rate = ((double)correct) / input_.size();
				std::cout << "test data correct rate: "
					<< rate * 100.0 << "% ("
					<< correct << " / " << input_.size() << ")" << std::endl;

				correct = 0;
				for (size_t idx = 0; idx < validate_input_.size(); ++idx) {
					auto result = net_.predict(validate_input_[idx]);
					bool predict_win = (result[0][0] > 0.0);
					bool actual_win = (validate_output_[idx][0] > 0.0);
					if (predict_win == actual_win) ++correct;
				}
				rate = ((double)correct) / validate_input_.size();
				std::cout << "validation correct rate: "
					<< rate * 100.0 << "% ("
					<< correct << " / " << validate_input_.size() << ")" << std::endl;
			}
		}

		void InitializePredict(std::string const& filename) {
			net_.load(filename);
		}

		double Predict(NeuralNetworkWrapper::IInputGetter * getter) {
			tiny_dnn::tensor_t input;
			GetInputData(getter, input);
			return net_.predict(input)[0][0];
		}

	private:
		void GetInputData(NeuralNetworkWrapper::IInputGetter * getter, tiny_dnn::tensor_t & data) {
			tiny_dnn::vec_t input1;
			AddHeroData(NeuralNetworkWrapper::kCurrent, getter, input1);
			AddHeroData(NeuralNetworkWrapper::kOpponent, getter, input1);
			data.push_back(std::move(input1));

			tiny_dnn::vec_t input2;
			AddMinionsData(NeuralNetworkWrapper::kCurrent, getter, input2);
			AddMinionsData(NeuralNetworkWrapper::kOpponent, getter, input2);
			data.push_back(std::move(input2));

			tiny_dnn::vec_t input3;
			AddStandAloneData(getter, input3);
			data.push_back(std::move(input3));
		}

		void AddHeroData(
			NeuralNetworkWrapper::FieldSide side,
			NeuralNetworkWrapper::IInputGetter * getter,
			tiny_dnn::vec_t & data)
		{
			double hp = getter->GetField(side, NeuralNetworkWrapper::kHeroHP) +
				getter->GetField(side, NeuralNetworkWrapper::kHeroArmor);
			data.push_back(NormalizeFromUniformDist(hp, 0.0, 30.0));
			//data.push_back(player["attack"].asFloat());
			//data.push_back(player["attackable"].asBool());
		}

		void AddMinionsData(
			NeuralNetworkWrapper::FieldSide side,
			NeuralNetworkWrapper::IInputGetter * getter,
			tiny_dnn::vec_t & data)
		{
			int rest = 7;
			for (int i = 0; i < (int)getter->GetField(side, NeuralNetworkWrapper::kMinionCount); ++i) {
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
			NeuralNetworkWrapper::FieldSide side,
			NeuralNetworkWrapper::IInputGetter * getter,
			int minion_idx,
			tiny_dnn::vec_t & data)
		{
			data.push_back(NormalizeFromUniformDist(getter->GetField(side, NeuralNetworkWrapper::kMinionHP, minion_idx), 1.0, 7.0));
			data.push_back(NormalizeFromUniformDist(getter->GetField(side, NeuralNetworkWrapper::kMinionMaxHP, minion_idx), 1.0, 7.0));
			data.push_back(NormalizeFromUniformDist(getter->GetField(side, NeuralNetworkWrapper::kMinionAttack, minion_idx), 0.0, 7.0));
			data.push_back(NormalizeBool(getter->GetField(side, NeuralNetworkWrapper::kMinionAttackable, minion_idx)));
			data.push_back(NormalizeBool(getter->GetField(side, NeuralNetworkWrapper::kMinionTaunt, minion_idx)));
			data.push_back(NormalizeBool(getter->GetField(side, NeuralNetworkWrapper::kMinionShield, minion_idx)));
			data.push_back(NormalizeBool(getter->GetField(side, NeuralNetworkWrapper::kMinionStealth, minion_idx)));
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
			NeuralNetworkWrapper::IInputGetter * getter,
			tiny_dnn::vec_t & data)
		{
			data.push_back(NormalizeFromUniformDist(getter->GetField(
				NeuralNetworkWrapper::kCurrent, NeuralNetworkWrapper::kResourceCurrent), 0, 10));
			data.push_back(NormalizeFromUniformDist(getter->GetField(
				NeuralNetworkWrapper::kCurrent, NeuralNetworkWrapper::kResourceTotal), 0, 10));
			data.push_back(NormalizeFromUniformDist(getter->GetField(
				NeuralNetworkWrapper::kCurrent, NeuralNetworkWrapper::kResourceOverloadNext), 0, 10));

			int cur_hand_count = (int)getter->GetField(NeuralNetworkWrapper::kCurrent, NeuralNetworkWrapper::kHandCount);
			data.push_back(NormalizeFromUniformDist(cur_hand_count, 0, 10));

			int cur_hand_playable = 0;
			for (int i = 0; i < cur_hand_count; ++i) {
				bool playable = getter->GetField(NeuralNetworkWrapper::kCurrent, NeuralNetworkWrapper::kHandPlayable, i);
				if (playable) {
					++cur_hand_playable;
				}
			}
			data.push_back(NormalizeFromUniformDist(cur_hand_playable, 0, 10));

			int hand_cards = 0;
			for (int i = 0; i < cur_hand_count; ++i) {
				data.push_back(NormalizeFromUniformDist(
					getter->GetField(NeuralNetworkWrapper::kCurrent, NeuralNetworkWrapper::kHandCost, i), 0, 10));
				++hand_cards;
			}
			while (hand_cards < 10) {
				data.push_back(NormalizeFromUniformDist(-1, 0, 10));
				++hand_cards;
			}

			int opn_hand_count = (int)getter->GetField(NeuralNetworkWrapper::kOpponent, NeuralNetworkWrapper::kHandCount);
			data.push_back(NormalizeFromUniformDist(opn_hand_count, 0, 10));

			data.push_back(NormalizeBool(
				getter->GetField(NeuralNetworkWrapper::kCurrent, NeuralNetworkWrapper::kHeroPowerPlayable)));
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
		std::vector<tiny_dnn::vec_t> output_;
		std::vector<tiny_dnn::tensor_t> validate_input_;
		std::vector<tiny_dnn::vec_t> validate_output_;
		tiny_dnn::network<tiny_dnn::graph> net_;
	};
}

NeuralNetworkWrapper::~NeuralNetworkWrapper()
{
	if (impl_) delete impl_;
}

void NeuralNetworkWrapper::InitializeTrain()
{
	if (!impl_) impl_ = new impl::NeuralNetworkWrapperImpl();
	impl_->InitializeTrain();
}

void NeuralNetworkWrapper::AddTrainData(IInputGetter * getter, int label, bool for_validate)
{
	impl_->AddTrainData(getter, label, for_validate);
}

void NeuralNetworkWrapper::Train()
{
	impl_->Train();
}

void NeuralNetworkWrapper::InitializePredict(std::string const& filename)
{
	if (!impl_) impl_ = new impl::NeuralNetworkWrapperImpl();
	impl_->InitializePredict(filename);
}

double NeuralNetworkWrapper::Predict(IInputGetter * getter)
{
	return impl_->Predict(getter);
}