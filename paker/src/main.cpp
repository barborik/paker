#include <iostream>
#include <fstream>
#include <filesystem>

const char* msg = "usage:\n	-c <dir> to compile\n	-l <PAK file> to list\n";

void compile(std::string dir)
{
	try
	{
		std::ofstream pakFile("out.PAK");

		// add magic numbers
		// https://en.wikipedia.org/wiki/Magic_number_(programming)
		pakFile.write("AB", sizeof(char) * 2);


		// determine content index header size
		size_t headerSize = 0;
		for (const auto& file : std::filesystem::directory_iterator(dir))
		{
			// char fileName[255] ... 255 bytes
			// size_t fileOffset ... 8 bytes
			// size_t fileSize ... 8 bytes
			// 1 block = 271 bytes
			headerSize += 255 + sizeof(size_t) * 2;
		}
		pakFile.write((char*)&headerSize, sizeof(size_t));

		// create content index header
		size_t offset = headerSize + sizeof(size_t) + sizeof(char) * 2;
		for (const auto& file : std::filesystem::directory_iterator(dir))
		{
			std::string path = file.path().string();
			std::string sname = path.substr(path.find_last_of("/\\") + 1);

			char* fileName = (char*)malloc(255);
			memcpy(fileName, sname.c_str(), sname.length());
			fileName[sname.length()] = 0; // 0-terminate the string to eliminate any funny business

			size_t fileOffset = offset;
			size_t fileSize = file.file_size();

			pakFile.write(fileName, 255);
			pakFile.write((char*)&fileOffset, sizeof(size_t));
			pakFile.write((char*)&fileSize, sizeof(size_t));

			offset += fileSize;
			free(fileName);
		}

		// paste data
		for (const auto& file : std::filesystem::directory_iterator(dir))
		{
			std::ifstream dataFile(file.path());

			char* data = (char*)malloc(file.file_size());
			dataFile.read(data, file.file_size());

			pakFile.write(data, file.file_size());
			free(data);

		}

		pakFile.close();
	}
	catch (std::filesystem::filesystem_error e)
	{
		std::cout << "directory \"" << dir << "\" not found\n";
		exit(-1);
	}
}

void list(std::string fname)
{
	std::ifstream pakFile(fname);

	// check for validity
	char magic[3];
	pakFile.read(magic, 2);
	magic[2] = 0;
	std::string smagic = magic;
	if (smagic != "AB")
	{
		std::cout << "PAK file not valid";
		exit(-1);
	}

	size_t headerSize;
	pakFile.read((char*)&headerSize, sizeof(size_t));

	for (int i = 0; i < headerSize / (255 + sizeof(size_t) * 2); i++)
	{
		char fileName[255];
		pakFile.read(fileName, 255);

		std::cout << fileName << "\n";
		pakFile.ignore(sizeof(size_t) * 2);
	}
	pakFile.close();
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << msg;
		return -1;
	}

	switch (argv[1][1])
	{
	case 'c':
		compile(argv[2]);
		break;
	case 'l':
		list(argv[2]);
		break;
	}

	return 0;
}