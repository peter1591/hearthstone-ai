#pragma once

#include <string>
#include <vector>
#include <memory>

#include "neural_net/IInputGetter.h"

namespace neural_net {
	class ITensorFlowAdaptor {
	public:
		ITensorFlowAdaptor() {}
		virtual ~ITensorFlowAdaptor() {}

		virtual void Initialize(const char* model_path) = 0;

		// State value is in range [-1, 1]
		// If first player is 100% wins, score = 1.0
		// If first player is 50% wins, 50%loss, score = 0.0
		// If first player is 100% loss, score = -1.0
		virtual float Predict(float* input) = 0;
	};

#ifdef TENSORFLOW_ADAPTOR_EXPORTING
#define TENSORFLOW_ADAPTOR_EXPORTS __declspec( dllexport )
#else
#define TENSORFLOW_ADAPTOR_EXPORTS __declspec( dllimport )
#endif

	extern "C" TENSORFLOW_ADAPTOR_EXPORTS ITensorFlowAdaptor* CreateTensorFlowAdaptor();
	extern "C" TENSORFLOW_ADAPTOR_EXPORTS void ReleaseTensorFlowAdaptor(ITensorFlowAdaptor* inst);

	class TensorFlowAdaptorManager {
	public:
		// not thread safe
		static void Initialize(std::string const& model_path) {
			GetInstanceRef() = CreateTensorFlowAdaptor();
			GetInstance().Initialize(model_path.c_str());
		}

		// not thread safe
		static void Release() {
			ReleaseTensorFlowAdaptor(&GetInstance());
		}

		static ITensorFlowAdaptor & GetInstance() {
			return *GetInstanceRef();
		}

	private:
		static ITensorFlowAdaptor* & GetInstanceRef() {
			static ITensorFlowAdaptor* inst = nullptr;
			return inst;
		}

	};
}
