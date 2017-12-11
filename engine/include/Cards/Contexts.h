#pragma once

namespace Cards {
	namespace Contexts {
		using CheckPlayable = FlowControl::onplay::context::CheckPlayable;
		using GetChooseOneOptions = FlowControl::onplay::context::GetChooseOneOptions;
		using SpecifiedTargetGetter = FlowControl::onplay::context::GetSpecifiedTarget;
		using OnPlay = FlowControl::onplay::context::OnPlay;
	}
}