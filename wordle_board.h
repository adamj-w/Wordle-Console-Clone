#ifndef WORDLE_BOARD_H 
#define WORDLE_BOARD_H

#include "dictionary.h"

typedef struct _board_struct {
	size_t current_row;
	size_t trys;
	size_t word_len;
	char* answer;
	char* board;
	unsigned char* fmt;
} board_t;

board_t* wordle_board_new_with_dict(size_t trys, dict_t* dict, size_t minWordLen, size_t maxWordLen);
board_t* wordle_board_new_with_answer(size_t trys, const char* answer);
void board_destroy(board_t* brd);

void board_print(board_t* brd);
int board_insert_guess(board_t* brd, const char* guess);

#endif
