#include <iostream>
#include "request_handler.h"

int main(void)
{
	std::string input;
	RequestHandler request_handler;

	if (!request_handler.Initialize()) return -1;

	while (!std::cin.eof())
	{
		// read until newline
		char c = (char)std::cin.get();
		if (c == '\r' || c == '\n')
		{
			if (!input.empty()) request_handler.Process(input);
			input.clear();
			continue;
		}

		input.push_back(c);
	}

	request_handler.Cleanup();

	return 0;
}