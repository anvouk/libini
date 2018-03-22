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

#ifndef INI_HPP
#define INI_HPP

#include <string>
#include <optional>

#if __has_include(<gsl/gsl>)
#  include <gsl/gsl>
#else
namespace gsl {
template<class T>
using owner = T;
}
#endif

namespace libini {

namespace c_api {

extern "C" {
#include "ini.h"
}

} // c_api

/**
 * a representation of an .ini file.
 * C++ class wrapper around the C libini api
 */
class ini {
public:
	ini() {
		ini_ = c_api::ini_create();
	}

	~ini() {
		c_api::ini_destroy(ini_);
	}

	ini(const ini& other) = default;
	ini& operator=(const ini& other) = default;

	ini(ini&& other) noexcept {
		this->ini_ = std::move(other.ini_);
	}

	ini& operator=(ini&& other) noexcept {
		this->ini_ = std::move(other.ini_);
		return *this;
	}

	/**
	 * check if the class initialized successfully and if it's ready to be used
	 *
	 * @return true		if everything is ok
	 */
	constexpr inline bool is_ready() const noexcept {
		return ini_ != nullptr;
	}

	/**
	 * add a unique key with its value
	 * 
	 * @param sec_name		the key's section
	 * @param key_name		the key's name
	 * @param val			the key's value
	 */
	inline void set(const std::string& sec_name, const std::string& key_name, int val) const noexcept {
		c_api::ini_add_key_i(ini_, sec_name.c_str(), key_name.c_str(), val);
	}

	/**
	 * add a unique key with its value
	 *
	 * @param sec_name		the key's section
	 * @param key_name		the key's name
	 * @param val			the key's value
	 */
	inline void set(const std::string& sec_name, const std::string& key_name, float val) const noexcept {
		c_api::ini_add_key_f(ini_, sec_name.c_str(), key_name.c_str(), val);
	}

	/**
	 * add a unique key with its value
	 *
	 * @param sec_name		the key's section
	 * @param key_name		the key's name
	 * @param val			the key's value
	 */
	inline void set(const std::string& sec_name, const std::string& key_name, const std::string& val) const noexcept {
		c_api::ini_add_key_str(ini_, sec_name.c_str(), key_name.c_str(), val.c_str());
	}

	/**
	 * get the key's value of type T (either 'int', 'float' or 'std::string')
	 * 
	 * @warning	if the key doesn't exist your program will crash. use get_opt()
	 *			if you're unsure whether the key does exist or not
	 * 
	 * @param sec_name		the key's section
	 * @param key_name		the key's name
	 * 
	 * @return				the key's value
	 */
	template<class T>
	inline T get(const std::string& sec_name, const std::string& key_name) const noexcept {
		static_assert(0, "T in get<T> can be only one of the following: 'int', 'float' or 'std::string'");
	}

	template<>
	inline int get(const std::string& sec_name, const std::string& key_name) const noexcept {
		return c_api::ini_get_key_i(ini_, sec_name.c_str(), key_name.c_str());
	}

	template<>
	inline float get(const std::string& sec_name, const std::string& key_name) const noexcept {
		return c_api::ini_get_key_f(ini_, sec_name.c_str(), key_name.c_str());
	}

	template<>
	inline std::string get(const std::string& sec_name, const std::string& key_name) const noexcept {
		char cstr[INI_STR_MAX_LENGTH];
		c_api::ini_get_key_str(ini_, sec_name.c_str(), key_name.c_str(), cstr, INI_STR_MAX_LENGTH);
		return std::string(cstr);
	}

	/**
	 * get the key's value of type T (either 'int', 'float' or 'std::string').
	 * if the key doesn't exist return an empty value
	 *
	 * @param sec_name		the key's section
	 * @param key_name		the key's name
	 *
	 * @return				nothing if the key doesn't exist
	 */
	template<class T>
	inline std::optional<T> get_opt(const std::string& sec_name, const std::string& key_name) const noexcept {
		static_assert(0, "T in get_opt<T> can be only one of the following: 'int', 'float' or 'std::string'");
	}

	template<>
	inline std::optional<int> get_opt(const std::string& sec_name, const std::string& key_name) const noexcept {
		if (exist(sec_name, key_name)) {
			return c_api::ini_get_key_i(ini_, sec_name.c_str(), key_name.c_str());
		}
		return std::optional<int>();
	}

	template<>
	inline std::optional<float> get_opt(const std::string& sec_name, const std::string& key_name) const noexcept {
		if (exist(sec_name, key_name)) {
			return c_api::ini_get_key_f(ini_, sec_name.c_str(), key_name.c_str());
		}
		return std::optional<float>();
	}

	template<>
	inline std::optional<std::string> get_opt(const std::string& sec_name, const std::string& key_name) const noexcept {
		if (exist(sec_name, key_name)) {
			char cstr[INI_STR_MAX_LENGTH];
			c_api::ini_get_key_str(ini_, sec_name.c_str(), key_name.c_str(), cstr, INI_STR_MAX_LENGTH);
			return std::string(cstr);
		}
		return std::optional<std::string>();
	}

	/**
	 * check if the key exists
	 * 
	 * @param sec_name		the key's section
	 * @param key_name		the key's name
	 * 
	 * @return				true if the key exists
	 */
	inline bool exist(const std::string& sec_name, const std::string& key_name) const noexcept {
		return static_cast<bool>(c_api::ini_does_key_exist(ini_, sec_name.c_str(), key_name.c_str()));
	}

	/**
	 * serialize the keys to a file
	 * 
	 * @param path		the file's path
	 * 
	 * @return			true if the serialization process succeeded
	 */
	inline bool serialize(const std::string& path) const noexcept {
		return static_cast<bool>(c_api::ini_serialize(ini_, path.c_str()));
	}

	/**
	 * parse an ini file and populate the class
	 * 
	 * @param path		the file's path
	 * 
	 * @return			true if the parsing process succeeded
	 */
	inline bool parse(const std::string& path) const noexcept {
		return static_cast<bool>(c_api::ini_parse(ini_, path.c_str()));
	}

private:
	gsl::owner<c_api::INI*> ini_;
};

} // libini

#endif // INI_HPP
