#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#include "dictionary.h"
#include "wordle_board.h"
#include "getopt.h"

board_t* board;

void sigint_handler(int param);
void print_help(void);

const char* get_input_sanitized(board_t*);
const char* get_input_valid(board_t*, dict_t* dict);

int main(int argc, char* argv[]) {
	puts("Wordle clone by Adam Warren (c) 2022");
	srand((unsigned int)time(NULL));

	int opt = 0;
	char* answer = NULL;
	char* dict_filename = NULL;
	unsigned int num_trys = 5, parse;
	while ((opt = getopt(argc, argv, "ha:d:t:")) != -1) {
		switch (opt) {
		case 'a':
			answer = optarg;
			break;
		case 'd':
			dict_filename = optarg;
			break;
		case 't':
			parse = strtoul(optarg, NULL, 10);
			if (errno == ERANGE || optarg[0] == '-') {
				puts("Unable to parse number after -t");
				print_help();
				return 0;
			}
			num_trys = parse;
			break;
		case '?':
		case 'h':
			print_help();
			return 0;
		}
	}

	signal(SIGINT, sigint_handler);

	dict_t* list;
	if (dict_filename) {
		list = dictionary_from_file(dict_filename, 0);
	} else {
		list = dictionary_from_file("engmix.txt", DICT_LOAD_LOWER_ONLY);
	}

	if (!list) {
		puts("Error loading dictionary!");
		return EXIT_FAILURE;
	}

	if (answer) {
		if (!dictionary_contains(list, answer)) {
			puts("User answer isn't contained in the provided dictionary!\n");
			return EXIT_FAILURE;
		}
		board = wordle_board_new_with_answer(num_trys, answer);
	}
	else {
		board = wordle_board_new_with_dict(num_trys, list, 5, 5);
	}

	if (!board) {
		return EXIT_FAILURE;
	}
	board_print(board);

	const char* input = get_input_valid(board, list);
	printf("Entered the word %s\n", input);

	int res;
	while ((res = board_insert_guess(board, input)) == 0) {
		board_print(board);
		free((void*)input);
		input = get_input_valid(board, list);
	}

	board_print(board);
	free((void*)input);

	if (res == 2) {
		printf("Better luck next time, the answer was \"%s\"", board->answer);
	}

	board_destroy(board);
	dictionary_destroy(list);
	return 0;
}

void print_help(void) {
	puts(" -a answer\t Answer to the board.");
	puts(" -d file  \t Location to a dictionary file in plain text form.");
	puts(" -t num   \t Number of rounds. (default=5)");
}

void sigint_handler(int param) {
	if(board) {
		printf("\nThe answer was \"%s\"\n", board->answer);
	}
	exit(0);
}

const char* get_input_valid(board_t* brd, dict_t* dict) {
	while (1) {
		const char* input = get_input_sanitized(brd);
		if (dictionary_contains(dict, input)) return input;
		free((void*)input);
		printf("Enter a valid english word.\n");
	}
}

const char* get_input_sanitized(board_t* brd) {
	size_t maxlen = brd->word_len + 50;
	char* str = (char*)malloc(maxlen);
	
	while (1) {
		printf("Enter a %zu letter word to try: ", brd->word_len);
		fgets(str, (int)maxlen, stdin);
		size_t len = strnlen(str, maxlen);
		if (len != brd->word_len + 1) { printf("The word entered was not %zu letters long!\n", brd->word_len); continue; }

		bool invalid = false;
		for (size_t i = 0; i < brd->word_len; i++) {
			char c = str[i];
			if ('A' <= c && c <= 'Z') {
				str[i] = c += 'a' - 'A';
			}
			
			if ('a' > c || c > 'z') {
				printf("Invalid character detected. Please enter only alphabetical letters!\n"); invalid = true; break;
			}
		}
		if (!invalid || str[brd->word_len] != '\n')
			break;
	}

	str[brd->word_len] = '\0';
	return str;
}
