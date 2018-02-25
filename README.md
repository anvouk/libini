# libini
A simple serialization/parser ini C library with a C++17 interface.

## Features
* `std::string`, `int` and `float` serialization and parsing
* simple ini manipulation (can only add keys)
* small and fast implementation
* no third-party library required

## Quick Start
### Serialization
```
libini::ini ini;
ini.set("", "GlobalKey", 123);
ini.set("Person.Attributes", "Name", "John");

ini.serialize("my_file.ini")
```
### Parsing
```
libini::ini ini;
ini.parse("my_file.ini")

int global_key = ini.get<int>("", "GlobalKey");
std::string name = ini.get<std::string>("Person.Attributes", "Name");

// try getting a non-existing key
auto some_key = ini.get_opt<int>("MySection", "SomeKey");
```
