#ifndef WORDLE_BOARD_H 
#define WORDLE_BOARD_H

#include "dictionary.hpp"

#include <cstddef>
#include <utility>

/*typedef struct _board_struct {
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
int board_insert_guess(board_t* brd, const char* guess);*/

class Board {
public:
	Board(uint8_t attempts, Dictionary* dict, size_t minWordLen, size_t maxWordLen);
	Board(uint8_t attempts, const std::string& answer);
	virtual ~Board() {}

	void Print() const;
	int InsertGuess(const std::string& guess);

	const std::string& GetAnswer() const { return answer; }
	size_t GetLength() const { return wordLen; }

private:
	enum class Fmt : uint8_t {
		Reset = 0, Grey = 1, Yellow = 2, Green = 3
	};

	std::string answer;
	size_t attempts, wordLen, currentRow;

	std::vector<std::pair<char, Fmt>> board;

};

#endif
