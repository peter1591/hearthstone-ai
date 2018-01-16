#pragma once

#include <random>
#include <mutex>

#include "alphazero/shared_data/circular_array.h"
#include "alphazero/shared_data/shared_ptr_item.h"

namespace alphazero
{
	namespace shared_data
	{
		class TrainingDataItem {
			// TODO
		};

		class TrainingData
		{
		public:
			TrainingData() : data_() {}

			void Initialize(size_t capacity_power_two) {
				data_.Initialize(capacity_power_two);
			}

			void Push(std::shared_ptr<TrainingDataItem> item) {
				data_.AllocateNext().Write(std::move(item));
			}

			void PushN(std::vector<std::shared_ptr<TrainingDataItem>> & items) {
				data_.AllocateNextN(items.size(), [&](size_t idx, SharedPtrItem<TrainingDataItem> & item) {
					item.Write(std::move(items[idx]));
				});
				items.clear();
			}

			// callback should make a copy of the data.
			template <class Callback>
			void RandomGet(std::mt19937 & random, Callback&& callback) {
				auto shared_ptr_item = data_.RandomGet(random).Get();
				callback(*shared_ptr_item);
			}

		private:
			CircularArray<SharedPtrItem<TrainingDataItem>> data_;
		};
	}
}