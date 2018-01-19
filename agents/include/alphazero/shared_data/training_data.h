#pragma once

#include <random>
#include <mutex>

#include "json/value.h"

#include "alphazero/shared_data/circular_array.h"
#include "alphazero/shared_data/shared_ptr_item.h"
#include "judge/json/Reader.h"

namespace alphazero
{
	namespace shared_data
	{
		class TrainingDataItem {
		public:
			TrainingDataItem(judge::json::NeuralNetInputGetter input, int label) :
				input_(input), label_(label)
			{}

			auto const& GetInput() const { return input_; }
			auto GetLabel() const { return label_; }

		private:
			judge::json::NeuralNetInputGetter input_;
			int label_;
		};

		class TrainingData
		{
		public:
			TrainingData() : data_(), size_() {}

			void Initialize(size_t capacity_power_two) {
				data_.Initialize(capacity_power_two);
				size_ = 0;
			}

			size_t GetCapacity() const {
				return data_.GetCapacity();
			}

			size_t GetSize() const { return size_.load(); }

			void Push(std::shared_ptr<TrainingDataItem> item) {
				data_.AllocateNext().Write(std::move(item));
				++size_;
			}

			template <class Callback>
			bool RandomGet(std::mt19937 & random, Callback&& callback) {
				// Fetch the item according to the size. This increase the probability to actually
				// fetch an item which is already written by a writer. But noted that we still has
				// a chance to get an empty (or very old data) if the writer is slow.
				int idx = random() % size_.load();

				// acquire ownership, so it will not be rotated out.
				auto shared_ptr_item = data_.RandomGet(idx).Get();
				
				// We still has a chance to read an empty data.
				if (!shared_ptr_item) return false;

				callback(*shared_ptr_item);
				return true;
			}

		private:
			CircularArray<SharedPtrItem<TrainingDataItem>> data_;
			std::atomic<size_t> size_;
		};
	}
}