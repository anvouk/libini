/*
 * The MIT License
 *
 * Copyright 2018 Andrea Vouk.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <iostream>

#include <libini/ini.hpp>

template<class T>
inline void print(T t)
{
	std::cout << t << std::endl;
}

void serialization()
{
	libini::ini ini;
	ini.set("", "GlobalKey", 1);

	ini.set("Person.Attributes", "Name", "John");
	ini.set("Person.Attributes", "Age", 32);

	ini.set("Person.RandomStats", "RandomNum", 654.956f);

	if (!ini.serialize("my_file.ini")) {
		print("libini failed serializing!");
	}
}

void parsing()
{
	libini::ini ini;
	if (!ini.parse("my_file.ini")) {
		print("libini failed parsing!");
	}

	int global_key = ini.get<int>("", "GlobalKey");
	print(global_key);

	std::string name = ini.get<std::string>("Person.Attributes", "Name");
	print(name);

	auto some_key = ini.get_opt<int>("MySection", "SomeKey");
	if (!some_key.has_value()) {
		print("some_key doesn't exist!");
	} else {
		print(some_key.value());
	}
}

int main()
{
	serialization();
	parsing();
}