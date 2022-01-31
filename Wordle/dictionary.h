#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct _dict_struct {
	size_t word_count;
	char** words;
	size_t bufsize;
	void* buffer;
	bool alphabetical;
} dict_t;

#define DICT_LOAD_DONT_ALPHABETIZE (0b1 << 0)
#define DICT_LOAD_LOWER_ONLY (0b1 << 1)
#define DICT_LOAD_LENGTH(__len) ((__len) << 2)
typedef uint32_t dictionary_load_flags_t;

dict_t* dictionary_from_file(const char* file, dictionary_load_flags_t flags);
dict_t* dictionary_from_str_array(size_t len, const char** list, dictionary_load_flags_t flags);
dict_t* dictionary_copy(const dict_t* dict);
void dictionary_save(dict_t* dict, const char* out_file);
void dictionary_destroy(dict_t* dict);

bool dictionary_contains(dict_t* dict, const char* str);
int64_t dictionary_index_of(dict_t* dict, const char* str);
const char* dictionary_get_word(const dict_t* dict, size_t index);
size_t dictionary_word_count(const dict_t* dict);

void dictionary_print_n(const dict_t* dict, size_t offset, size_t n);

void dictionary_alphabetize(dict_t* dict);
void dictionary_sanitize_rough(dict_t* dict);
void dictionary_sanitize_to_length(dict_t* dict, size_t length);

#endif
