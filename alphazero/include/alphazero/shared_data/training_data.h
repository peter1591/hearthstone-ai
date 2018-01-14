#pragma once

#include <random>

#include "alphazero/shared_data/circular_buffer.h"
#include "alphazero/shared_data/batch_items.h"

namespace alphazero
{
	namespace shared_data
	{
		class TrainingDataItem {
			// TODO
		};

		// Several self-players running on several threads are generating data here
		// One trainer is getting data from here, to train a neural network
		// Use circular buffer as underlying container.
		
		// Each item in the circular buffer is a batch of items, so writers can acquire less locks
		// This has a drawback that, an item in the circular buffer might have been allocated to a writer,
		// but not yet be fully-filled.
		// We can adjust the size of the batch-of-items, to control this probability under a predefined probability x%.
		// Say, the circular buffer can contains N items, consisting of B batch-of-items, and each batch contains N/B items
		// Assume we have W writers. In worst case, we have W not-fully-filled batch-items, which is with probability W/N.
		// To control the probability under x (say, 1%), we need to have W/B < x
		// --> So, each batch size N/B < x*N/W

		// Thread safety: Yes
		class TrainingData
		{
		public:
			void Initialize(size_t capacity, size_t writers, float not_fully_filled_prob_upper_limit) {
				// As described above, each batch size should be N/B < x*N/W.
				// Take the maximum value, to reduce the writer lock as much as possible.
				int batch_size = (int)(not_fully_filled_prob_upper_limit * capacity / writers);
				if (batch_size < 1) batch_size_ = 1;
				else batch_size_ = (size_t)batch_size;

				batch_items_ = capacity / batch_size_;
				if (GetCapacity() < capacity) ++batch_items_;
				assert(GetCapacity() >= capacity);

				items_.Initialize(batch_items_, BatchItems<TrainingDataItem>(batch_size_));
			}

			size_t GetCapacity() const { return batch_items_ * batch_size_; }
			size_t GetBatchItems() const { return batch_items_; }
			size_t GetBatchSize() const { return batch_size_; }

			BatchItems<TrainingDataItem> & AllocateNext() {
				return items_.AllocateNext();
			}

			BatchItems<TrainingDataItem> & Get(size_t idx) {
				return items_.Get(idx);
			}
			BatchItems<TrainingDataItem> const& Get(size_t idx) const {
				return items_.Get(idx);
			}

		private:
			size_t batch_items_; // number of batches
			size_t batch_size_; // size of each batch
			size_t writers_;

			CircularBuffer<BatchItems<TrainingDataItem>> items_;
		};

		class TrainingDataWriter
		{
		public:
			TrainingDataWriter() : data_(nullptr), current_batch_(nullptr) {}
			TrainingDataWriter(TrainingData & data) : data_(&data), current_batch_(nullptr) {}

			void PushBack(TrainingDataItem const& item) {
				if (!current_batch_ || current_batch_->IsFull()) {
					current_batch_ = &data_->AllocateNext();
				}
				assert(!current_batch_->IsFull());
				current_batch_->PushBack(item);
			}

		private:
			TrainingData * data_;
			BatchItems<TrainingDataItem> * current_batch_;
		};

		class TrainingDataManager
		{
		public:
			void Initialize(size_t capacity, size_t writers, float not_fully_filled_prob_upper_limit) {
				data_.Initialize(capacity, writers, not_fully_filled_prob_upper_limit);
			}

			TrainingDataWriter GetWriter() {
				return TrainingDataWriter(data_);
			}

			// return nullptr if failed to get a item.
			// Note that, according to design, there's a small chance to failed to get an item even if there are some items.
			TrainingDataItem const* RandomGet(std::mt19937 & random) const {
				assert(data_.GetBatchItems() > 0);
				int batch_idx = random() % data_.GetBatchItems();
				assert(batch_idx >= 0);
				assert(batch_idx < data_.GetBatchItems());
				auto const& batch_item = data_.Get(batch_idx);
				
				if (!batch_item.IsFull()) {
					// writer is still on it
					return nullptr;
				}

				assert(batch_item.GetCapacity() == data_.GetBatchSize());
				int item_idx = random() % batch_item.GetCapacity();
				return &batch_item.Get(item_idx);
			}

		private:
			TrainingData data_;
		};
	}
}