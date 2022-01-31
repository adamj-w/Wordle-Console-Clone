#include <cstring>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <csignal>

#include "dictionary.hpp"
#include "wordle_board.hpp"
#include "getopt.h"

Board* board;

void print_help(void);
extern "C" void sigint_handler(int);

const char* get_input_sanitized(Board*);
const char* get_input_valid(Board*, Dictionary* dict);

int main(int argc, char* argv[]) {
	std::cout << "Wordle clone by Adam Warren (c) 2022" << std::endl;
	srand((unsigned int)time(NULL));

	int opt = 0;
	char* answer = NULL;
	char* dict_filename = NULL;
	unsigned int num_trys = 6, parse;
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

	std::signal(SIGINT, sigint_handler);

	Dictionary* list;
	if (dict_filename) {
		list = new Dictionary(std::filesystem::path(dict_filename));
	} else {
		list = new Dictionary("engmix.txt", Dictionary::LoadFlags::LOWER_ONLY);
	}

	if (answer) {
		if(!list->Contains(answer)) {
			std::cout << "User answer isn't contained in the provided dictionary!" << std::endl;
			return EXIT_FAILURE;
		}
		board = new Board(num_trys, answer);
	}
	else {
		board = new Board(num_trys, list, 5, 5);
	}

	
	board->Print();

	const char* input = get_input_valid(board, list);
	std::cout << "Entered the word " << input << std::endl;

	int res;
	while ((res = board->InsertGuess(input)) == 0) {
		board->Print();
		delete input;
		input = get_input_valid(board, list);
	}

	board->Print();
	delete input;

	if (res == 2) {
		std::cout << "Better luck next time, the answer was " << std::quoted(board->GetAnswer()) << std::endl;
	}
	
	delete board;
	delete list;
	return 0;
}

void sigint_handler(int param) {
	if (board) std::cout << "The answer was " << std::quoted(board->GetAnswer()) << std::endl;
	exit(0);
}

void print_help(void) {
	std::cout << " -a answer\t Answer to the board." << std::endl;
	std::cout << " -d file  \t Location to a dictionary file in plain text form." << std::endl;
	std::cout << " -t num   \t Number of rounds. (default=5)" << std::endl;
}

const char* get_input_valid(Board* brd, Dictionary* dict) {
	while (1) {
		const char* input = get_input_sanitized(brd);
		if (dict->Contains(input)) return input;
		delete input;
		std::cout << "Enter a valid english word." << std::endl;
	}
}

const char* get_input_sanitized(Board* brd) {
	size_t maxlen = brd->GetLength() + 50;
	char* str = new char[maxlen];
	
	while (1) {
		printf("Enter a %zu letter word to try: ", brd->GetLength());
		fgets(str, (int)maxlen, stdin);
		size_t len = strnlen(str, maxlen);
		if (len != brd->GetLength() + 1) { printf("The word entered was not %zu letters long!\n", brd->GetLength()); continue; }

		bool invalid = false;
		for (size_t i = 0; i < brd->GetLength(); i++) {
			char c = str[i];
			if ('A' <= c && c <= 'Z') {
				str[i] = c += 'a' - 'A';
			}
			
			if ('a' > c || c > 'z') {
				printf("Invalid character detected. Please enter only alphabetical letters!\n"); invalid = true; break;
			}
		}
		if (!invalid || str[brd->GetLength()] != '\n')
			break;
	}

	str[brd->GetLength()] = '\0';
	return str;
}
