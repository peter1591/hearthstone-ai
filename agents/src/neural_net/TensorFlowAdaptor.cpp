#include "neural_net/TensorFlowAdaptor.h"

#include "tensorflow/core/public/session.h"
#include "tensorflow/core/platform/env.h"

using namespace tensorflow;

namespace neural_net {
	class TensorFlowAdaptor : public ITensorFlowAdaptor {
	public:
		TensorFlowAdaptor();
		~TensorFlowAdaptor();

		void Initialize(const char* model_path);

		// State value is in range [-1, 1]
		// If first player is 100% wins, score = 1.0
		// If first player is 50% wins, 50%loss, score = 0.0
		// If first player is 100% loss, score = -1.0
		float Predict(float* input);

	private:
		float Predict(std::vector<float> const& input);

	private:
		GraphDef * graph_def_;
		tensorflow::Session * session_;
		std::unique_ptr<std::vector<tensorflow::Tensor>> outputs_;
	};

	TensorFlowAdaptor::TensorFlowAdaptor() : session_(nullptr) {
	}

	TensorFlowAdaptor::~TensorFlowAdaptor() {
		if (session_) session_->Close();
	}

	void TensorFlowAdaptor::Initialize(const char* model_path)
	{
		graph_def_ = new GraphDef();
		Status status = ReadBinaryProto(Env::Default(), model_path, graph_def_);
		if (!status.ok()) {
			throw std::runtime_error(status.ToString());
		}

		outputs_.reset(new std::vector<Tensor>());
	}

	float TensorFlowAdaptor::Predict(float* input)
	{
		std::vector<float> converted;

		// TODO: do conversion
		converted.resize(140);

		return Predict(converted);
	}

	float TensorFlowAdaptor::Predict(std::vector<float> const& input)
	{
		auto options = SessionOptions();
		options.config.set_inter_op_parallelism_threads(1);
		options.config.set_intra_op_parallelism_threads(1);

		Status status = NewSession(options, &session_);
		if (!status.ok()) {
			throw std::runtime_error(status.ToString());
		}

		status = session_->Create(*graph_def_);
		if (!status.ok()) {
			throw std::runtime_error(status.ToString());
		}

		Tensor input_tensor(Tensor(DT_FLOAT, TensorShape({ 1, (int)input.size() })));
		for (int i = 0; i < (int)input.size(); ++i) {
			input_tensor.matrix<float>()(0, i) = input[i];
		}

		std::vector<std::pair<string, tensorflow::Tensor>> inputs = {
			{ "Placeholder", input_tensor },
		};

		status = session_->Run(inputs, { "final/BiasAdd" }, {}, outputs_.get());
		if (!status.ok()) {
			throw std::runtime_error(status.ToString());
		}

		session_->Close();
		delete session_;
		session_ = nullptr;

		auto output = (*outputs_)[0].scalar<float>();
		return output();
	}

	ITensorFlowAdaptor* CreateTensorFlowAdaptor() {
		return new TensorFlowAdaptor();
	}

	void ReleaseTensorFlowAdaptor(ITensorFlowAdaptor* inst) {
		delete inst;
	}
}