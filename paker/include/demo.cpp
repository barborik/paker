#include <iostream>
#include "paker.h"

int demo()
{
	size_t fileSize;
	char* data = pak::load("test.txt", "out.PAK", &fileSize);

	// check for errors
	if (!data)
	{
		std::cout << "file not found";
		return 0;
	}

	data[fileSize] = 0; // because the text file isnt 0-terminated we have to do it ourselves
	std::cout << data << "\n";

	return 0;
}