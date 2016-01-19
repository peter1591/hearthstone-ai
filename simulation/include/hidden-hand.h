#ifndef HIDDEN_HAND_H
#define HIDDEN_HAND_H

#include <sstream>
#include <string>

class HiddenHand
{
	public:
		std::string GetDebugString() const;

		int GetCardCount() const { return this->total_cards; }

	private:
		int total_cards;
};

inline std::string HiddenHand::GetDebugString() const
{
	std::ostringstream oss;
	oss << "Count: " << this->total_cards;
	return oss.str();
}

#endif
