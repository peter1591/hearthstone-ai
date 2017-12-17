#pragma once

#include "FlowControl/FlowContext.h"

namespace mcts
{
	struct Result {
		enum Type {
			kResultInvalid,
			kResultWin,
			kResultLoss, // including draw
			kResultNotDetermined
		};

		Result() : type_(kResultInvalid), score_(0.0) {}
		Result(Type type) : type_(type), score_(0.0) {
			if (type_ == kResultWin) score_ = 1.0;
			else if (type_ == kResultLoss) score_ = -1.0;
		}

		static Result ConvertFrom(FlowControl::Result result) {
			// The player AI is helping is defined to be the first player
			switch (result) {
			case FlowControl::Result::kResultFirstPlayerWin:
				return Result::kResultWin;

			case FlowControl::Result::kResultSecondPlayerWin:
			case FlowControl::Result::kResultDraw:
				return Result::kResultLoss;

			case FlowControl::Result::kResultNotDetermined:
				return Result::kResultNotDetermined;

			case FlowControl::Result::kResultInvalid:
			default:
				return Result::kResultInvalid;
			}
		}

		// Score is in range [-1.0, 1.0]
		// If the first player has 100% chance to win, set the value to 1.0
		// If the first plyaer has 100% chance to loss, set the value to -1.0
		// If it's a 50-50, set the value to 0.0
		explicit Result(double score) : type_(kResultInvalid), score_(score) {
			assert(score <= 1.0);
			assert(score >= -1.0);
			if (score > 0.0) type_ = kResultWin;
			else type_ = kResultLoss;
		}

		Type type_;
		double score_; // range 0~1
	};

	enum Stage {
		kStageSelection,
		kStageSimulation
	};
}