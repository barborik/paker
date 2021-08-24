#pragma once

#include <fstream>

namespace pak
{
	// returns pointer to the data of the file requested
	// returns 0 on fail
	// free memory after youre done using it
	char* load(const char* fileName, const char* pakName, size_t* fileSize)
	{
		std::ifstream pakFile(pakName);

		// discard magic numbers
		pakFile.ignore(sizeof(char) * 2);

		size_t headerSize;
		pakFile.read((char*)&headerSize, sizeof(size_t));

		size_t fileOffset;
		for (int i = 0; i < headerSize / (255 + sizeof(size_t) * 2); i++)
		{
			char fname[255];
			pakFile.read(fname, 255);

			std::string sname = fname;
			if (fileName == sname)
			{
				pakFile.read((char*)&fileOffset, sizeof(size_t));
				pakFile.read((char*)fileSize, sizeof(size_t));
				break;
			}

			pakFile.ignore(sizeof(size_t) * 2);

			if (i == headerSize / (255 + sizeof(size_t) * 2) - 1)
			{
				return 0;
			}
		}
		pakFile.clear();
		pakFile.seekg(0, pakFile.beg);
		pakFile.ignore(fileOffset);

		char* data = (char*)malloc(*fileSize);
		pakFile.read(data, *fileSize);

		pakFile.close();
		return data;
	}
}
