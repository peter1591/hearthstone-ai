#pragma once

namespace state
{
	namespace Events
	{
		class HandlersContainerController
		{
		public:
			HandlersContainerController() : remove_(false) {}

			void Remove() { remove_ = true; }

			bool IsRemoved() const { return remove_; }

		private:
			bool remove_;
		};
	}
}