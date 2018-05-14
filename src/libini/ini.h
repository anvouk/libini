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
 * @file ini.h
 * 
 * libini core C API.
 */

#ifndef INI_H
#define INI_H

#include <stddef.h>	/* for size_t */

#define INI_VERSION 0x1001

#if defined(_MSC_VER) && !defined(INI_BUILD_STATIC)
#  ifdef INI_INTERNAL
#    define INIAPI __declspec(dllexport)
#  else
#    define INIAPI __declspec(dllimport)
#  endif
#else
#  define INIAPI extern
#endif

#define INI_STR_MAX_LENGTH	128

struct INI;
typedef struct INI INI;

INIAPI INI* ini_create	(void);
INIAPI void ini_destroy	(INI* ini);

INIAPI int	ini_does_key_exist(INI* ini, const char* sec_name, const char* key_name);

INIAPI void		ini_add_key_i	(INI* ini, const char* sec_name, const char* key_name, int val);
INIAPI void		ini_add_key_f	(INI* ini, const char* sec_name, const char* key_name, float val);
INIAPI void		ini_add_key_str	(INI* ini, const char* sec_name, const char* key_name, const char* val);

INIAPI int		ini_get_key_i	(INI* ini, const char* sec_name, const char* key_name);
INIAPI float	ini_get_key_f	(INI* ini, const char* sec_name, const char* key_name);
INIAPI void		ini_get_key_str	(INI* ini, const char* sec_name, const char* key_name, char* out_buff, size_t buff_size);

INIAPI int	ini_serialize	(INI* ini, const char* path);
INIAPI int	ini_parse		(INI* ini, const char* path);

/* C11 support required */
#if !defined(__cplusplus) && __STDC_VERSION__ >= 201112L

#define ini_add_key(ini, sec, key, val) _Generic((val), \
		int:			ini_add_key_i,	\
		float:			ini_add_key_f,	\
		default:		ini_add_key_str \
	)(ini, sec, key, val)

#endif

#endif /* INI_H */