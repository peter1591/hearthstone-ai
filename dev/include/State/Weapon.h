#pragma once

namespace state
{
	namespace board
	{
		class Weapon
		{
		public:
			void Equip(CardRef card_ref)
			{
				card_ref_ = card_ref;
			}

			void Destroy()
			{
				card_ref_.Invalidate();
			}

			bool IsEquipped() const
			{
				return card_ref_.IsValid();
			}

		private:
			CardRef card_ref_;
		};
	}
}