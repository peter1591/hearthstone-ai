#pragma once

namespace Cards
{
	class Card_CS2_106 : public WeaponCardBase<Card_CS2_106>
	{
	public:
		static constexpr int id = Cards::ID_CS2_106;

		Card_CS2_106()
		{
		}
	};
}

REGISTER_WEAPON_CARD_CLASS(Cards::Card_CS2_106)
