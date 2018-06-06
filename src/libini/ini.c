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

#include "libini/ini.h"

#include <stdio.h>	/* fprintf() */
#include <stdlib.h>	/* malloc(), free() */
#include <string.h>	/* strcmp(), strcpy(), strcpy_s() */
#include <ctype.h>	/* isspace(), iscntrl(), isalpha() */

#define KVAL_TYPE_UNDEFINED	0
#define KVAL_TYPE_INT		1
#define KVAL_TYPE_FLOAT		2
#define KVAL_TYPE_STR		3

typedef struct INI_KEY {
	char key_name[INI_STR_MAX_LENGTH];
	union {
		int ival;
		float fval;
		char sval[INI_STR_MAX_LENGTH];
	};
	int t_val;
} INI_KEY;

typedef struct INI_SECTION {
	char sec_name[INI_STR_MAX_LENGTH];
	INI_KEY** keys;
	int keys_count;
} INI_SECTION;

struct INI {
	INI_SECTION** secs;
	int secs_count;
};

/* handle here what happens when memory allocation fails */
#define alloc_check(x, msg) if (!x) { assert(0 && msg); exit(EXIT_FAILURE); }

/*------------------------------------------------------------------------------
	INI MANIPULATION
------------------------------------------------------------------------------*/

static void key_set_i(INI_KEY* key, int val)
{
	key->ival = val;
	key->t_val = KVAL_TYPE_INT;
}

static void key_set_f(INI_KEY* key, float val)
{
	key->fval = val;
	key->t_val = KVAL_TYPE_FLOAT;
}

static void key_set_str(INI_KEY* key, const char* val)
{
	strcpy_s(key->sval, INI_STR_MAX_LENGTH, val);
	key->t_val = KVAL_TYPE_STR;
}

static INI_KEY* key_create(const char* name)
{
	INI_KEY* key = malloc(sizeof(INI_KEY));
	alloc_check(key, "key creation: malloc failed\n");
	strcpy_s(key->key_name, INI_STR_MAX_LENGTH, name);
	key->t_val = KVAL_TYPE_UNDEFINED;
	return key;
}

static void key_destroy(INI_KEY* key)
{
	free(key);
}

static INI_SECTION* sec_create(const char* name)
{
	INI_SECTION* sec = malloc(sizeof(INI_SECTION));
	alloc_check(sec, "section creation: malloc failed\n");
	strcpy_s(sec->sec_name, INI_STR_MAX_LENGTH, name);
	sec->keys = NULL;
	sec->keys_count = 0;
	return sec;
}

static void sec_destroy(INI_SECTION* sec)
{
	for (int i = sec->keys_count - 1; i >= 0; i--) {
		key_destroy(sec->keys[i]);
	}
	free(sec->keys);
	free(sec);
}

static void sec_add_key(INI_SECTION* sec, INI_KEY* key)
{
	sec->keys = realloc(sec->keys, sizeof(INI_KEY));
	alloc_check(sec->keys, "adding a key: realloc failed\n");
	sec->keys[sec->keys_count] = key;
	sec->keys_count++;
}

static INI_KEY* sec_get_key(INI_SECTION* sec, const char* key_name)
{
	INI_KEY* key;
	for (int i = 0; i < sec->keys_count; i++) {
		key = sec->keys[i];
		if (strcmp(key->key_name, key_name) == 0) {
			return key;
		}
	}
	return NULL;
}

INI* ini_create(void)
{
	INI* ini = malloc(sizeof(INI));
	alloc_check(ini, "ini initialization: malloc failed\n");
	ini->secs = NULL;
	ini->secs_count = 0;
	return ini;
}

void ini_destroy(INI* ini)
{
	for (int i = ini->secs_count - 1; i >= 0; i--) {
		sec_destroy(ini->secs[i]);
	}
	free(ini->secs);
	free(ini);
}

static void ini_add_sec(INI* ini, INI_SECTION* sec)
{
	ini->secs = realloc(ini->secs, sizeof(INI_SECTION));
	alloc_check(ini->secs, "adding a section: realloc failed\n");
	ini->secs[ini->secs_count] = sec;
	ini->secs_count++;
}

static INI_SECTION* ini_get_section(INI* ini, const char* sec_name)
{
	INI_SECTION* sec;
	for (int i = 0; i < ini->secs_count; i++) {
		sec = ini->secs[i];
		if (strcmp(sec->sec_name, sec_name) == 0) {
			return sec;
		}
	}
	return NULL;
}

static void ini_add_key_generic(INI* ini, const char* sec_name,
	const char* key_name, INI_KEY* key)
{
	INI_SECTION* sec = ini_get_section(ini, sec_name);
	if (sec) {
		sec_add_key(sec, key);
	} else {
		sec = sec_create(sec_name);
		sec_add_key(sec, key);
		ini_add_sec(ini, sec);
	}
}

void ini_add_key_i(INI* ini, const char* sec_name, const char* key_name,
	int val)
{
	INI_KEY* key = key_create(key_name);
	key_set_i(key, val);
	ini_add_key_generic(ini, sec_name, key_name, key);
}

void ini_add_key_f(INI* ini, const char* sec_name, const char* key_name,
	float val)
{
	INI_KEY* key = key_create(key_name);
	key_set_f(key, val);
	ini_add_key_generic(ini, sec_name, key_name, key);
}

void ini_add_key_str(INI* ini, const char* sec_name, const char* key_name,
	const char* val)
{
	INI_KEY* key = key_create(key_name);
	key_set_str(key, val);
	ini_add_key_generic(ini, sec_name, key_name, key);
}

int ini_does_key_exist(INI* ini, const char* sec_name, const char* key_name)
{
	INI_SECTION* sec = ini_get_section(ini, sec_name);
	if (sec) {
		INI_KEY* key = sec_get_key(sec, key_name);
		if (key) {
			return 1;
		}
	}
	return 0;
}

int ini_get_key_i(INI* ini, const char* sec_name, const char* key_name)
{
	INI_SECTION* sec = ini_get_section(ini, sec_name);
	INI_KEY* key = sec_get_key(sec, key_name);
	return key->ival;
}

float ini_get_key_f(INI* ini, const char* sec_name, const char* key_name)
{
	INI_SECTION* sec = ini_get_section(ini, sec_name);
	INI_KEY* key = sec_get_key(sec, key_name);
	return key->fval;
}

void ini_get_key_str(INI* ini, const char* sec_name, const char* key_name,
	char* out_buff, size_t buff_size)
{
	INI_SECTION* sec = ini_get_section(ini, sec_name);
	INI_KEY* key = sec_get_key(sec, key_name);
	strcpy_s(out_buff, buff_size, key->sval);
}

/*------------------------------------------------------------------------------
	SERIALIZATION
------------------------------------------------------------------------------*/

#define foreach_section(ini) \
	INI_SECTION* sec = ini->secs[0]; \
	for (int isec = 0; isec < ini->secs_count; isec++, sec = ini->secs[isec])

#define foreach_key(sec) \
	INI_KEY* key = sec->keys[0]; \
	for (int ikey = 0; ikey < sec->keys_count; ikey++, key = sec->keys[ikey])

int ini_serialize(INI* ini, const char* path)
{
	FILE* stream = fopen(path, "w");
	if (!stream) return 0;
	foreach_section(ini) {
		if (sec->sec_name[0] != '\0') {
			fprintf(stream, "[%s]\n", sec->sec_name);
		}
		foreach_key(sec) {
			switch (key->t_val) {
				case KVAL_TYPE_INT:
					fprintf(stream, "%s=%i\n", key->key_name, key->ival);
					break;
				case KVAL_TYPE_FLOAT:
					fprintf(stream, "%s=%f\n", key->key_name, key->fval);
					break;
				case KVAL_TYPE_STR:
					fprintf(stream, "%s=%s\n", key->key_name, key->sval);
					break;
				case KVAL_TYPE_UNDEFINED:
				default:
					break;
			}
		}
		fputc('\n', stream);
	}
	fclose(stream);
	return 1;
}

/*------------------------------------------------------------------------------
	PARSING
------------------------------------------------------------------------------*/

#define getc_buffer(c, buff, pos) ((c) = (buff)[(pos)])

static void parse_buffer_stream(INI* ini, const char* path, char** out_buff)
{
	FILE* stream = fopen(path, "r");
	if (!stream) return;

	fseek(stream, 0L, SEEK_END);
	long fsize = ftell(stream);
	rewind(stream);

	/* allocate memory for entire content */
	*out_buff = calloc(sizeof(char), fsize + 1);
	if (!*out_buff) {
		fclose(stream);
		fputs("memory alloc fails", stderr);
		exit(EXIT_FAILURE);
	}

	/* copy the file into the buffer */
	fread(*out_buff, sizeof(char), fsize, stream);

	fclose(stream);
}

static void parse_key(INI* ini, char* buff, long* pos, INI_SECTION** last_sec)
{
	if (!(*last_sec)) {
		*last_sec = sec_create("");
		ini_add_sec(ini, *last_sec);
	}

	char key_name[INI_STR_MAX_LENGTH] = "";
	int keypos = 0;
	char c;
	while (getc_buffer(c, buff, *pos) != '=') {
		key_name[keypos] = c;
		keypos++;
		(*pos)++;
	}
	key_name[keypos] = '\0';
	(*pos)++;

	INI_KEY* key = key_create(key_name);

	long valpos = 0;
	memset(key_name, 0, INI_STR_MAX_LENGTH);

	int is_str = 0;
	int is_float = 0;
	while (getc_buffer(c, buff, *pos) != '\n') {
		if (isspace(c) || isalpha(c)) is_str = 1;
		if (c == '.') is_float = 1;
		key_name[valpos] = c;
		valpos++;
		(*pos)++;
	}
	key_name[valpos] = '\0';

	if (is_str) {
		key_set_str(key, key_name);
	} else if (is_float) {
		key_set_f(key, (float)atof(key_name));
	} else {
		key_set_i(key, atoi(key_name));
	}
	sec_add_key(*last_sec, key);
}

static INI_SECTION* parse_section(INI* ini, char* buff, long* pos)
{
	/* parse section name */
	char sec_name[INI_STR_MAX_LENGTH] = "";
	int secpos = 0;
	char c;
	while (getc_buffer(c, buff, *pos) != ']') {
		sec_name[secpos] = c;
		secpos++;
		(*pos)++;
	}
	sec_name[secpos] = '\0';
	(*pos)++;

	/* set section */
	INI_SECTION* sec = sec_create(sec_name);
	ini_add_sec(ini, sec);
	return sec;
}

int ini_parse(INI* ini, const char* path)
{
	char* buffer;
	parse_buffer_stream(ini, path, &buffer);

	INI_SECTION* last_sec = NULL;

	long pos = 0;
	char c;
	while (getc_buffer(c, buffer, pos) != EOF) {
		if (isspace(c)) {
			pos++;
			continue;
		}
		if (iscntrl(c)) {
			return 1;
		}

		if (c == ';') {
			pos++;
			while (getc_buffer(c, buffer, pos) != '\n') {
				if (iscntrl(c)) {
					return 1;
				}
				pos++;
			}
		} else if (isalpha(c)) {
			parse_key(ini, buffer, &pos, &last_sec);
		} else if (c == '[') {
			pos++;
			last_sec = parse_section(ini, buffer, &pos);
		} else {
			return 0;
		}
		pos++;
	}

	return 1;
}
