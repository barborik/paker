#pragma once

#include <fstream>

namespace pak
{
	// returns pointer to the data of the file requested
	// returns 0 on fail
	// free memory after youre done using it
	char* load(const char* filename, const char* pakPath, size_t* fileSize)
	{
		std::ifstream pakFile(pakPath);

		// check for validity
		char magic[3];
		pakFile.read(magic, 2);
		magic[2] = 0;
		std::string smagic = magic;
		if (smagic != "AB")
		{
			return 0;
		}

		size_t hSize;
		pakFile.read((char*)&hSize, sizeof(size_t));

		// find file by filename and get its offset
		size_t fOffset;
		for (int i = 0; i < hSize / (255 + sizeof(size_t) * 2); i++)
		{
			char fname[255];
			pakFile.read(fname, 255);

			std::string sname = fname;
			if (fname == sname)
			{
				pakFile.read((char*)&fOffset, sizeof(size_t));
				pakFile.read((char*)fileSize, sizeof(size_t));
				break;
			}

			pakFile.ignore(sizeof(size_t) * 2);
		}

		// no offset found, return 0
		if (fOffset == NULL)
		{
			return 0;
		}

		pakFile.seekg(fOffset);

		char* data = (char*)malloc(*fileSize);
		pakFile.read(data, *fileSize);

		pakFile.close();
		return data;
	}
}
