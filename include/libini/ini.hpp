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

/**
 * @file ini.hpp
 * 
 * C++ interface for the libini C API.
 */

#ifndef INI_HPP
#define INI_HPP

#include <string>
#include <optional>

namespace libini
{
namespace c_api
{

extern "C"
{
#include "ini.h"
}

} // c_api

/**
 * a representation of a .ini file.
 *
 * C++ class wrapper around the C libini api
 */
class ini
{
public:
	ini()
	{
		m_ini = c_api::ini_create();
	}

	~ini()
	{
		c_api::ini_destroy(m_ini);
	}

	ini(const ini& other) = default;
	ini& operator=(const ini& other) = default;

	ini(ini&& other) noexcept
	{
		this->m_ini = std::move(other.m_ini);
	}

	ini& operator=(ini&& other) noexcept
	{
		this->m_ini = std::move(other.m_ini);
		return *this;
	}

	/**
	 * Check whether or not the class initialization succeeded and it's
	 * ready to be used.
	 *
	 * @return true if everything is ok
	 */
	constexpr inline bool is_ready() const noexcept
	{
		return m_ini != nullptr;
	}

	/**
	 * Add a unique key and its value.
	 *
	 * @param sec_name  The key's section
	 * @param key_name  The key's name
	 * @param val       The key's value
	 */
	inline void set(const std::string& sec_name, const std::string& key_name, int val) const noexcept
	{
		c_api::ini_add_key_i(m_ini, sec_name.c_str(), key_name.c_str(), val);
	}

	/**
	 * Add a unique key with its value.
	 *
	 * @param sec_name  The key's section
	 * @param key_name  The key's name
	 * @param val       The key's value
	 */
	inline void set(const std::string& sec_name, const std::string& key_name, float val) const noexcept
	{
		c_api::ini_add_key_f(m_ini, sec_name.c_str(), key_name.c_str(), val);
	}

	/**
	 * Add a unique key with its value.
	 *
	 * @param sec_name  The key's section
	 * @param key_name  The key's name
	 * @param val       The key's value
	 */
	inline void set(const std::string& sec_name, const std::string& key_name, const std::string& val) const noexcept
	{
		c_api::ini_add_key_str(m_ini, sec_name.c_str(), key_name.c_str(), val.c_str());
	}

	/**
	 * Get the key's value of type T.
	 *
	 * @warning Make sure the key does exist before calling this function.
	 *          Use get_opt() if you can't be certain of it.
	 *
	 * @param sec_name  The key's section
	 * @param key_name  The key's name
	 *
	 * @tparam T  The key type. Either <code>int</code>, <code>float</code>
	 *            or <code>std::string</code>
	 *
	 * @return The key's value
	 */
	template<class T>
	inline T get(const std::string& sec_name, const std::string& key_name) const noexcept
	{
		static_assert(0, "T in get<T> can be only one of the following: 'int', 'float' or 'std::string'");
	}

	template<>
	inline int get(const std::string& sec_name, const std::string& key_name) const noexcept
	{
		return c_api::ini_get_key_i(m_ini, sec_name.c_str(), key_name.c_str());
	}

	template<>
	inline float get(const std::string& sec_name, const std::string& key_name) const noexcept
	{
		return c_api::ini_get_key_f(m_ini, sec_name.c_str(), key_name.c_str());
	}

	template<>
	inline std::string get(const std::string& sec_name, const std::string& key_name) const noexcept
	{
		char cstr[INI_STR_MAX_LENGTH];
		c_api::ini_get_key_str(m_ini, sec_name.c_str(), key_name.c_str(), cstr, INI_STR_MAX_LENGTH);
		return std::string(cstr);
	}

	/**
	 * Get the key's value of type T. If the key doesn't exist return an
	 * empty value.
	 *
	 * @param sec_name  The key's section
	 * @param key_name  The key's name
	 *
	 * @tparam T  The key type. Either <code>int</code>, <code>float</code>
	 *            or <code>std::string</code>
	 *
	 * @return An empty <code>std::optional<T></code> when the key doesn't
	 *         exist
	 */
	template<class T>
	inline std::optional<T> get_opt(const std::string& sec_name, const std::string& key_name) const noexcept
	{
		static_assert(0, "T in get_opt<T> can be only one of the following: 'int', 'float' or 'std::string'");
	}

	template<>
	inline std::optional<int> get_opt(const std::string& sec_name, const std::string& key_name) const noexcept
	{
		if (exist(sec_name, key_name)) {
			return c_api::ini_get_key_i(m_ini, sec_name.c_str(), key_name.c_str());
		}
		return std::optional<int>();
	}

	template<>
	inline std::optional<float> get_opt(const std::string& sec_name, const std::string& key_name) const noexcept
	{
		if (exist(sec_name, key_name)) {
			return c_api::ini_get_key_f(m_ini, sec_name.c_str(), key_name.c_str());
		}
		return std::optional<float>();
	}

	template<>
	inline std::optional<std::string> get_opt(const std::string& sec_name, const std::string& key_name) const noexcept
	{
		if (exist(sec_name, key_name)) {
			char cstr[INI_STR_MAX_LENGTH];
			c_api::ini_get_key_str(m_ini, sec_name.c_str(), key_name.c_str(), cstr, INI_STR_MAX_LENGTH);
			return std::string(cstr);
		}
		return std::optional<std::string>();
	}

	/**
	 * Check whether or not the key exists.
	 *
	 * @param sec_name  The key's section
	 * @param key_name  The key's name
	 *
	 * @return true if the key exists
	 */
	inline bool exist(const std::string& sec_name, const std::string& key_name) const noexcept
	{
		return static_cast<bool>(c_api::ini_does_key_exist(m_ini, sec_name.c_str(), key_name.c_str()));
	}

	/**
	 * Serialize to an ini file.
	 *
	 * @param path  The file's path
	 *
	 * @return true when the serialization process succeeded
	 */
	inline bool serialize(const std::string& path) const noexcept
	{
		return static_cast<bool>(c_api::ini_serialize(m_ini, path.c_str()));
	}

	/**
	 * Parse an ini file and populate this class instance with its data.
	 *
	 * @param path  The file's path
	 *
	 * @return true when the parsing process succeeded
	 */
	inline bool parse(const std::string& path) const noexcept
	{
		return static_cast<bool>(c_api::ini_parse(m_ini, path.c_str()));
	}

private:
	c_api::INI* m_ini;
};

} // libini

#endif // INI_HPP