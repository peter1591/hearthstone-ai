#ifndef SECRETS_H
#define SECRETS_H

#include <vector>
#include "secret.h"

class Secrets
{
	public:
		Secrets();

	private:
		std::vector<Secret> secrets;
};

inline Secrets::Secrets()
{
	this->secrets.reserve(5);
}

#endif
