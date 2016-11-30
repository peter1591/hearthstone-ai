#pragma once

namespace FlowControl
{
	class IRandomGenerator
	{
	public:
		virtual int Get(int exclusive_max) = 0;
		virtual int Get(int min, int max) = 0;
	};
}