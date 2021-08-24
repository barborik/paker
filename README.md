# paker
simple PAK compiler and API

## usage
```-c <dir>``` compile directory<br>
```-l <PAK file>``` list contents of a PAK file generated by paker

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
