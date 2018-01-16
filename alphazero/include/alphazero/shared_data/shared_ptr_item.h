#pragma once

#include <atomic>
#include <memory>

namespace alphazero
{
	namespace shared_data
	{
		// a lock-free item holder
		template <class T>
		class SharedPtrItem
		{
		public:
			SharedPtrItem() : item_() {}

			void Write(std::shared_ptr<T> item) {
				// If no reader is holding item_, it is destroyed and pointed to ,
				// (destroy since no variable catching return value)
				// If a reader is holding item_, the lifetime is transferred there automatically
				std::atomic_exchange(&item_, item);
			}

			std::shared_ptr<T> Get() const {
				return item_;
			}

		private:
			std::shared_ptr<T> item_;
		};
	}
}