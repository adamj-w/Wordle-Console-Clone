#include "dictionary.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#ifdef PLATFORM64
#define SIZE_FMT "%llu"
#else
#define SIZE_FMT "%lu"
#endif

#define TMP_BUF_LENGTH 0x1000

void _dictionary_clean_list(dict_t* dict, size_t num_removed);

dict_t* dictionary_from_file(const char* file, dictionary_load_flags_t flags) {
	dict_t* out = (dict_t*)malloc(sizeof(dict_t));
	if (out == NULL) return NULL;
	memset(out, 0, sizeof(dict_t));

	FILE* f = fopen(file, "r");
	if (!f) { printf("Failed to open file at \"%s\"\n", file); return NULL; }

	{
		char* buf = (char*)malloc(TMP_BUF_LENGTH);
		if (!buf) { printf("Out of memory!\n"); fclose(f); return out; }

		size_t read;
		while ((read = fread(buf, 1, TMP_BUF_LENGTH, f)) > 0) {
			for (size_t i = 0; i < read; ++i) {
				if (buf[i] == '\n') out->word_count++;
			}
		}

		free((void*)buf);
	}

	printf("Found " SIZE_FMT " words\n", out->word_count);
	out->bufsize = (size_t)ftell(f);
	fseek(f, 0, SEEK_SET);

	out->words = (char**)malloc(sizeof(char*) * out->word_count);
	if (out->words == NULL) { printf("Out of memory!\n"); fclose(f); free((void*)out); return NULL; }
	out->buffer = malloc(out->bufsize);
	if (out->buffer == NULL) { printf("Out of memory!\n"); fclose(f); free((void*)out->words); free((void*)out); return NULL; }

	size_t read = fread(out->buffer, 1, out->bufsize, f);
	fclose(f);

	if ((flags >> 2) != 0) {
		size_t tgt_len = flags >> 2;
		char* buf = (char*)out->buffer;
		size_t offset = 0, word_idx = 0;
		bool valid = true;
		for (size_t i = 0; i < out->bufsize; ++i) {
			if (buf[i] == '\n') {
				if (valid && i - offset == tgt_len) {
					out->words[word_idx++] = &buf[offset];
				} else
					out->word_count--; 

				buf[i] = '\0';
				offset = i + 1;
				valid = true;
			}
			else if (flags & DICT_LOAD_LOWER_ONLY && !islower(buf[i])) {
				valid = false;
			}
		}
	} else {
		char* buf = (char*)out->buffer;
		size_t offset = 0, word_idx = 0;
		bool valid = true;
		for (size_t i = 0; i < out->bufsize; ++i) {
			if (buf[i] == '\n') {
				if (valid) {
					out->words[word_idx++] = &buf[offset];
				}
				else
					out->word_count--;

				buf[i] = '\0';
				offset = i + 1;
				valid = true;
			}
			else if ((flags & DICT_LOAD_LOWER_ONLY) && ('a' > buf[i] || buf[i] > 'z')) {
				valid = false;
			}
		}
	}

	if (!(flags & DICT_LOAD_DONT_ALPHABETIZE)) {
		dictionary_alphabetize(out);
	}

	return out;
}

dict_t* dictionary_from_str_array(size_t len, const char** list, dictionary_load_flags_t flags) {
	if (len == 0) return NULL;

	dict_t* out = (dict_t*)malloc(sizeof(dict_t));
	if (out == NULL) return NULL;
	memset(out, 0, sizeof(dict_t));

	size_t bufsize = 0;
	for (size_t i = 0; i < len; ++i) {
		bufsize += strlen(list[i]) + 1;
	}
	out->bufsize = bufsize;
	out->buffer = malloc(bufsize);
	if (out->buffer == NULL) { printf("Out of memory!\n"); free((void*)out); return NULL; }
	memset(out->buffer, 0, bufsize);
	out->word_count = len;
	out->words = (char**)calloc(len, sizeof(char*));
	if (out->words == NULL) { printf("Out of memory!\n"); free(out->buffer); free((void*)out); return NULL; }

	size_t tgt_len = flags >> 2;
	if (tgt_len != 0) {
		size_t offset = 0;
		for (size_t i = 0; i < len; ++i) {
			strncpy((char*)out->buffer + offset, list[i], bufsize - offset);
			size_t len_str = strlen(list[i]);
			if (len_str == tgt_len)
				out->words[i] = (char*)out->buffer + offset;
			else
				out->word_count--;
			offset += len_str + 1;
		}
	} else {
		size_t offset = 0;
		for (size_t i = 0; i < len; ++i) {
			strncpy((char*)out->buffer + offset, list[i], bufsize - offset);
			out->words[i] = (char*)out->buffer + offset;
			offset += strlen(list[i]) + 1;
		}
	}

	if (flags & DICT_LOAD_LOWER_ONLY) {
		dictionary_sanitize_rough(out);
	}

	if (!(flags & DICT_LOAD_DONT_ALPHABETIZE)) {
		dictionary_alphabetize(out);
	}

	return out;
}

dict_t* dictionary_copy(const dict_t* dict) {
	if (!dict) return NULL;

	dict_t* copy = (dict_t*)malloc(sizeof(dict_t));
	if (!copy) { printf("Out of memory!\n"); return NULL; }
	memset(copy, 0, sizeof(dict_t));

	if (!dict->buffer || !dict->words) return copy;

	copy->words = (char**)calloc(dict->word_count, sizeof(char*));
	if (!copy->words) { printf("Out of memory!\n"); return NULL; }

	copy->buffer = malloc(dict->bufsize);
	if (!copy->buffer) { printf("Out of memory!\n"); free((void*)copy->words);  return NULL; }

	copy->bufsize = dict->bufsize;
	copy->word_count = dict->word_count;

	memcpy(copy->buffer, dict->buffer, dict->bufsize);

	for (size_t i = 0; i < dict->word_count; ++i) {
		uintptr_t offset = (uintptr_t)dict->words[i] - (uintptr_t)dict->buffer;
		copy->words[i] = (char*)((uintptr_t)copy->buffer + offset);
	}

	return copy;
}

void dictionary_save(dict_t* dict, const char* out_file) {
	if (!dict || !dict->words) return;
	FILE* fp = fopen(out_file, "w");
	if (!fp) { printf("Unable to open file \"%s\"", out_file); return; }

	for (size_t i = 0; i < dict->word_count; ++i) {
		fprintf(fp, "%s\n", dict->words[i]);
	}

	fclose(fp);
}

void dictionary_destroy(dict_t* dict) {
	if (!dict) return;

	if (dict->buffer) {	free(dict->buffer);	}

	if (dict->words) { free((void*)dict->words); }

	free((void*)dict);
}

bool dictionary_contains(dict_t* dict, const char* str)
{
	return dictionary_index_of(dict, str) >= 0;
}

int _dictionary_index_of_compar(const void* a, const void* b) {
	const char* aa = *(const char**)a;
	const char* bb = *(const char**)b;
	while (*aa) {
		if (tolower(*aa) != tolower(*bb))
			break;
		aa++;
		bb++;
	}
	return *(const unsigned char*)aa - *(const unsigned char*)bb;
}

int64_t dictionary_index_of(dict_t* dict, const char* str) {
	if (!dict->alphabetical) dictionary_alphabetize(dict);

	void* addr = bsearch(&str, dict->words, dict->word_count, sizeof(char*), _dictionary_index_of_compar);
	if (addr == NULL) return -1;

	uintptr_t offset = (uintptr_t)addr - (uintptr_t)dict->words;
	return offset / sizeof(char*);
}

const char* dictionary_get_word(const dict_t* dict, size_t index) {
	if (index >= dict->word_count) return NULL;
	if (!dict->words) return NULL;
	return dict->words[index];
}

size_t dictionary_word_count(const dict_t* dict) {
	return dict->word_count;
}

void dictionary_print_n(const dict_t* dict, size_t offset, size_t n) {
	if (!dict || !dict->words) return;
	if (offset + n > dict->word_count) { n = dict->word_count - offset; } // force inbounds

	for (size_t i = offset; i < offset + n; ++i) {
		printf("%s\n", dict->words[i]);
	}
}

void _dictionary_clean_list(dict_t* dict, size_t num_removed) {
	size_t new_len = dict->word_count - num_removed;
	char** new_words = (char**)malloc(sizeof(char*) * new_len);
	if (new_words == NULL) { printf("Out of memory!\n"); exit(EXIT_FAILURE); }

	for (size_t i = 0, j = 0; i < dict->word_count; ++i) {
		if (dict->words[i] != NULL) {
			new_words[j++] = dict->words[i];
		}
	}

	free((void*)dict->words);
	dict->words = new_words;
	dict->word_count = new_len;
}

int _dictionary_alphabetize_compar(const void* a, const void* b) {
	const char* aa = *(const char**)a;
	const char* bb = *(const char**)b;
	while (*aa) {
		if (tolower(*aa) != tolower(*bb))
			break;
		aa++;
		bb++;
	}
	return *(const unsigned char*)aa - *(const unsigned char*)bb;
}

void dictionary_alphabetize(dict_t* dict) {
	qsort(dict->words, dict->word_count, sizeof(char*), _dictionary_alphabetize_compar);
	dict->alphabetical = true;
}

void dictionary_sanitize_rough(dict_t* dict) {
	if (!dict || !dict->words) return;

	size_t num_removed = 0;
	for (size_t i = 0; i < dict->word_count; ++i) {
		size_t len = strlen(dict->words[i]);
		for (size_t j = 0; j < len; ++j) {
			if (!islower(dict->words[i][j])) {
				dict->words[i] = NULL;
				num_removed++;
				break;
			}
		}
	}

	_dictionary_clean_list(dict, num_removed);
}

void dictionary_sanitize_to_length(dict_t* dict, size_t length) {
	if (!dict || !dict->words) return;

	size_t num_removed = 0;
	for (size_t i = 0; i < dict->word_count; ++i) {
		size_t len = strlen(dict->words[i]);
		if (len != length) {
			dict->words[i] = NULL;
			num_removed++;
		}
	}

	_dictionary_clean_list(dict, num_removed);
}
