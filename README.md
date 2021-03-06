# paker
simple PAK compiler and API

## usage
```-c <dir>``` compile directory<br>
```-d <PAK file>``` decompile PAK file<br>
```-l <PAK file>``` list contents of a PAK file generated by paker

## paker file structure
```
char magic_numbers[2]
size_t content_index_header_size

content_index_header
{
	char file_name[255]
	size_t file_offset
	size_t file_size
	...
	1 block = 271 bytes
}

content
{
	char* data
}
```

## paker API
to use the paker API simply include [paker.h](https://github.com/barborik/paker/blob/master/paker/include/paker.h) in your project
### read data from a PAK file generated by paker
```cpp
size_t fileSize;
char* data = pak::load("test.txt", "out.PAK", &fileSize);

// check for errors
if (!data)
{
  std::cout << "file not found";
  exit(0);
}

data[fileSize] = 0; // because the text file isnt 0-terminated we have to do it ourselves
std::cout << data << "\n";
```
