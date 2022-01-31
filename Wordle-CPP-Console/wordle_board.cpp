#include "wordle_board.hpp"

#include <iostream>
#include <algorithm>

/*
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char* _formats[] = { "\033[0m", "\033[30;1m", "\033[33;1m", "\33[32;1m" };

#define BOARD(__i, __j) brd->board[__i * brd->word_len + __j]
#define FMT(__i, __j) brd->fmt[__i * brd->word_len + __j]

board_t* wordle_board_new_with_dict(size_t trys, dict_t* dict, size_t minWordLen, size_t maxWordLen) {
	if (minWordLen > maxWordLen) return NULL;
	size_t len = (rand() % (maxWordLen - minWordLen + 1)) + minWordLen;

	board_t* brd = (board_t*)malloc(sizeof(board_t));
	if (!brd) { printf("Out of memory!\n"); return NULL; }
	memset(brd, 0, sizeof(board_t));

	brd->trys = trys;
	brd->word_len = len;

	brd->board = (char*)malloc(trys * len);
	if (!brd->board) { printf("Out of memory!\n"); free((void*)brd); return NULL; }

	brd->fmt = (unsigned char*)malloc(trys * len);
	if (!brd->fmt) { printf("Out of memory!\n"); free((void*)brd->board); free((void*)brd); return NULL; }

	memset(brd->fmt, 0, trys * len);
	memset(brd->board, ' ', trys * len);

	brd->answer = (char*)malloc(len + 1);
	if (!brd->answer) { printf("Out of memory!\n"); board_destroy(brd); return NULL; }

	dict_t* dict_copy = dictionary_copy(dict);
	if (!dict_copy) return NULL;
	dictionary_sanitize_to_length(dict_copy, len);
	const char* answer = dictionary_get_word(dict_copy, rand() % dict_copy->word_count);

	strncpy(brd->answer, answer, len);
	brd->answer[len] = '\0';

	dictionary_destroy(dict_copy);

	brd->current_row = 0;

	return brd;
}

board_t* wordle_board_new_with_answer(size_t trys, const char* answer) {
	size_t len = strlen(answer);
	if (len == 0) return NULL;

	board_t* brd = (board_t*)malloc(sizeof(board_t));
	if (!brd) { printf("Out of memory!\n"); return NULL; }
	memset(brd, 0, sizeof(board_t));

	brd->trys = trys;
	brd->word_len = len;

	brd->board = (char*)malloc(trys * len);
	if (!brd->board) { printf("Out of memory!\n"); free((void*)brd); return NULL; }

	brd->fmt = (unsigned char*)malloc(trys * len);
	if (!brd->fmt) { printf("Out of memory!\n"); free((void*)brd->board); free((void*)brd); return NULL; }

	memset(brd->fmt, 0, trys * len);
	memset(brd->board, ' ', trys * len);

	brd->answer = (char*)malloc(len + 1);
	if (!brd->answer) { printf("Out of memory!\n"); board_destroy(brd); return NULL; }
	strncpy(brd->answer, answer, len);
	brd->answer[len] = '\0';

	brd->current_row = 0;

	return brd;
}

void board_destroy(board_t* brd) {
	if (!brd) return;
	if (brd->board) free((void*)brd->board);
	if (brd->fmt) free((void*)brd->fmt);
	if (brd->answer) free((void*)brd->answer);
	free((void*)brd);
}

void board_print(board_t* brd) {
	for (size_t i = 0; i < brd->trys; ++i) {
		for (size_t j = 0; j < brd->word_len; ++j) {
			printf("----");
		}
		printf("\n");

		for (size_t j = 0; j < brd->word_len; j++) {
			char c = BOARD(i, j);
			printf("| %s%c%s ", _formats[FMT(i, j)], c, _formats[0]);
		}
		
		printf("|\n");
	}
	for (size_t j = 0; j < brd->word_len; ++j) {
		printf("----");
	}
	printf("\n");
#ifdef _DEBUG
	printf("Answer is \"%s\", current row: %zu\n", brd->answer, brd->current_row);
#endif
}

int board_insert_guess(board_t* brd, const char* guess) {
	for (size_t i = 0; i < brd->word_len; ++i) {
		BOARD(brd->current_row, i) = guess[i];
		char* idx = strchr(brd->answer, guess[i]);
		if (idx == &brd->answer[i] || brd->answer[i] == guess[i]) {
			FMT(brd->current_row, i) = 3;
		}
		else if (idx != NULL) {
			FMT(brd->current_row, i) = 2;
		}
		else {
			FMT(brd->current_row, i) = 1;
		}
	}
	if (strcmp(guess, brd->answer) == 0) {
		printf("You win!!\n");
		return 1;
	}
	if (++brd->current_row >= brd->trys) {
		printf("You lose!\n");
		return 2;
	}
	return 0;
}
*/

const std::string_view _formats[] = { "\033[0m", "\033[30;1m", "\033[33;1m", "\033[32;1m" };

Board::Board(uint8_t trys, Dictionary* dict, size_t minWordLen, size_t maxWordLen)
	: currentRow(0), attempts(trys) {
	if (minWordLen > maxWordLen) std::swap(minWordLen, maxWordLen);
	size_t len = (rand() % (maxWordLen - minWordLen + 1)) + minWordLen;
	wordLen = len;

	board.resize(attempts * len);
	std::for_each(board.begin(), board.end(), [](std::pair<char, Board::Fmt>& val) {
		val = { ' ', Fmt::Reset };
	});

	auto* dict_copy = new Dictionary(*dict);
	dict_copy->SanitizeToLength(len);
	answer = dict_copy->GetWord(rand() % dict_copy->WordCount());
	delete dict_copy;
}

Board::Board(uint8_t trys, const std::string& answer) 
	: attempts(trys), currentRow(0) {
	size_t len = answer.size();
	auto check = std::find_if(answer.cbegin(), answer.cend(), [](char c) { return !std::isalpha(c); });
	if (len == 0 || check != answer.cend()) throw std::invalid_argument("Please pass a valid answer argument");
	wordLen = len;

	board.resize(len * attempts);
	std::for_each(board.begin(), board.end(), [](std::pair<char, Board::Fmt>& val) {
		val = { ' ', Fmt::Reset };
	});

	this->answer = answer;
}

void Board::Print() const {
	for (size_t i = 0; i < attempts; ++i) {
		for (size_t j = 0; j < wordLen; ++j) {
			std::cout << "----";
		}
		std::cout << std::endl;

		for (size_t j = 0; j < wordLen; j++) {
			const auto& pair = board[i * wordLen + j];
			std::cout << "| " << _formats[(size_t)pair.second] << pair.first << _formats[0] << " ";
		}

		std::cout << '|' << std::endl;
	}
	for (size_t j = 0; j < wordLen; ++j) {
		std::cout << "----";
	}
	std::cout << std::endl;
#ifdef _DEBUG
	std::cout << "Answer is " << std::quoted(answer) << ", current row: " << currentRow << std::endl;
#endif
}

int Board::InsertGuess(const std::string& guess) {
	for (size_t i = 0; i < wordLen; ++i) {
		auto& pair = board[currentRow * wordLen + i];
		pair.first = guess[i];
		const auto& idx = std::find(answer.cbegin(), answer.cend(), guess[i]);
		if (answer[i] == guess[i]) {
			pair.second = Fmt::Green;
		}
		else if (idx != answer.cend()) {
			pair.second = Fmt::Yellow;
		}
		else {
			pair.second = Fmt::Grey;
		}
	}
	if (answer == guess) {
		std::cout << "You win!!" << std::endl;
		return 1;
	}
	if (++currentRow >= attempts) {
		std::cout << "You lose!" << std::endl;
		return 2;
	}
	return 0;
}
