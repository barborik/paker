#include <iostream>
#include <fstream>
#include <filesystem>

const char* msg = "usage:\n	-c <dir> ... compile directory\n	-d <PAK file> ... decompile PAK file\n	-l <PAK file> ... list contents of a PAK file\n";

void readBlock(std::ifstream& pakFile, int index, std::string& sfname, size_t* fOffset, size_t* fSize)
{
	pakFile.seekg(index * (sizeof(char) * 255 + sizeof(size_t) * 2) + sizeof(char) * 2 + sizeof(size_t)); // skip magic numbers, header size and blocks before index
	char fname[255];
	pakFile.read(fname, sizeof(char) * 255);
	pakFile.read((char*)fOffset, sizeof(size_t));
	pakFile.read((char*)fSize, sizeof(size_t));
	sfname = fname;
}

void writeNextBlock(std::ofstream& pakFile, std::string& sfname, size_t fOffset, size_t fSize)
{
	char fname[255];
	memcpy(fname, sfname.c_str(), sfname.length());
	fname[sfname.length()] = 0; // 0-terminate the string to eliminate any funny business

	pakFile.write(fname, 255);
	pakFile.write((char*)&fOffset, sizeof(size_t));
	pakFile.write((char*)&fSize, sizeof(size_t));
}

void compile(std::string dir)
{
	try
	{
		std::ofstream pakFile("out.PAK", std::ios::binary);

		// add magic numbers
		// https://en.wikipedia.org/wiki/Magic_number_(programming)
		pakFile.write("AB", sizeof(char) * 2);

		// write content index header size
		size_t hSize = std::distance(std::filesystem::directory_iterator(dir), std::filesystem::directory_iterator{});
		hSize *= 255 + sizeof(size_t) * 2;
		pakFile.write((char*)&hSize, sizeof(size_t));

		// create content index header
		size_t offset = hSize + sizeof(size_t) + sizeof(char) * 2;
		for (const auto& file : std::filesystem::directory_iterator(dir))
		{
			std::string path = file.path().string();
			std::string sfname = path.substr(path.find_last_of("/\\") + 1);

			writeNextBlock(pakFile, sfname, offset, file.file_size());
			offset += file.file_size();
		}

		// paste data
		for (const auto& file : std::filesystem::directory_iterator(dir))
		{
			std::ifstream dataFile(file.path(), std::ios::binary);

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

void decompile(std::string pakPath)
{
	std::filesystem::remove_all("out");
	std::filesystem::create_directory("out");
	std::ifstream pakFile(pakPath, std::ios::binary);

	// check for validity
	char magic[3];
	pakFile.read(magic, 2);
	magic[2] = 0;
	std::string smagic = magic;
	if (smagic != "AB")
	{
		std::cout << "PAK file not valid\n";
		exit(-1);
	}

	size_t hSize;
	pakFile.read((char*)&hSize, sizeof(size_t));

	for (int i = 0; i < hSize / (255 + sizeof(size_t) * 2); i++)
	{
		std::string sfname;
		size_t fOffset, fSize;
		readBlock(pakFile, i, sfname, &fOffset, &fSize);

		std::ofstream outFile("out/" + sfname, std::ios::binary);
		pakFile.seekg(fOffset);

		char* data = (char*)malloc(fSize);
		pakFile.read(data, fSize);
		outFile.write(data, fSize);

		free(data);
		outFile.close();
	}
}

void list(std::string pakPath)
{
	std::ifstream pakFile(pakPath, std::ios::binary);

	// check for validity
	char magic[3];
	pakFile.read(magic, 2);
	magic[2] = 0;
	std::string smagic = magic;
	if (smagic != "AB")
	{
		std::cout << "PAK file not valid\n";
		exit(-1);
	}

	size_t hSize;
	pakFile.read((char*)&hSize, sizeof(size_t));
	for (int i = 0; i < hSize / (255 + sizeof(size_t) * 2); i++)
	{
		std::string sfname;
		size_t fOffset, fSize;
		readBlock(pakFile, i, sfname, &fOffset, &fSize);

		std::cout << sfname << "\n";
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
	case 'd':
		decompile(argv[2]);
		break;
	case 'l':
		list(argv[2]);
		break;
	}

	return 0;
}