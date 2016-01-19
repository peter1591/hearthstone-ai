#ifndef MINION_H
#define MINION_H

class Minion
{
	public:
		Minion();

		int card_id;
		int attack;
		int hp;
		int max_hp;

		bool IsValid() const;
};

inline Minion::Minion() : card_id(0)
{

}

inline bool Minion::IsValid() const
{
	return this->card_id != 0;
}

#endif
