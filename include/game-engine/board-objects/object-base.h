#pragma once

namespace GameEngine {
class ObjectBase
{
public:
	ObjectBase() {}
	virtual ~ObjectBase() {}

	virtual int GetHP() const = 0;

	virtual bool Attackable() const = 0;
	virtual int GetAttack() const = 0;

	virtual void TakeDamage(int damage, bool poisonous) = 0;
	virtual int GetForgetfulCount() const = 0;

	virtual void AttackedOnce() = 0;

	virtual void SetFreezed() = 0;
	virtual bool IsFreezeAttacker() const = 0;
	virtual bool IsFreezed() const = 0;

	virtual bool IsPoisonous() const = 0;

	/*
	virtual int GetMaxHP() = 0;
	*/
};

} // namespace GameEngine
