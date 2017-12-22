#pragma once

namespace Cards {
	namespace Contexts {
		using CheckPlayable = engine::FlowControl::onplay::context::CheckPlayable;
		using GetChooseOneOptions = engine::FlowControl::onplay::context::GetChooseOneOptions;
		using SpecifiedTargetGetter = engine::FlowControl::onplay::context::GetSpecifiedTarget;
		using OnPlay = engine::FlowControl::onplay::context::OnPlay;
	}
}