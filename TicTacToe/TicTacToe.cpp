#include <iostream>
#include <vector>
using namespace std;

class TicTacToe {
private:

	const int BOARD_COLUMNS = 3; // the number of columns on the board
	const int BOARD_ROWS = 3; // the number of rows on the board
	const int MAX_SCORE = 10; // the number to make the below algorithm optimal (that is, use every mistake of the player immediately to its advantage) - it should be >= (MAX_DEPTH + 1)
	const int MAX_DEPTH = 9; // the maximum depth of the searched tree
	const int DRAW_SCORE = 0; // the score to be returned when a draw is evaluated
	const int MIN_DEPTH_FOR_SOLUTION = 4; // the minimum depth (number of turns) at which to determine when to start checking for a winner (a winner is present when one of the players has at least 3 characters on the board)
	const int MAX_CONSECUTIVE_AI = 3; // if the sum of either a row from rows vector, a column from columns vector or the value of primaryDiagoanl/secondaryDiagonal equals this value, then the AI has put three consecutive characters => he wins
	const int MAX_CONSECUTIVE_PLAYER = -3; // same as above but for the player
	const char EMPTY_CELL = ' '; // how an empty cell should be represented
	const char FIRST_CHARACTER = 'X'; // the first character (either the player or the AI takes it)
	const char SECOND_CHARACTER = 'O'; // the second character
	const char PLAYER_WIN_SYMBOL = 'W'; // when we determine the winner, the function that is doing that returns this constant whenever the user has won
	const char PLAYER_LOSE_SYMBOL = 'L'; // when the user has lost
	const char DRAW_SYMBOL = 'D'; // if the resulting game is a draw
	char playerCharacter; // the character which the AI uses
	char aiCharacter; // the character which the player uses
	vector<vector<char>> board; // represents the board
	pair<int, int> aiMove; // the cell which the AI shall take
	bool isPersonsTurn;
	vector<int> rows; // contains the values of the rows in the board (+1 if there is an AI character on the row, -1 if there is a player character on the row)
	vector<int> columns; // same as above but for the columns
	int primaryDiagonal; // same as above but for the primaryDiagonal (note that it is one in this case, because we need 3 consecutive characters)
	int secondaryDiagonal; // same as above but for the secondaryDiagonal

	void printBoard(const int depth) {
		cout << "<-----" << " Turn " << depth + 1 << " ----->" << endl << endl;

		for (int row = 0; row < BOARD_ROWS + 1; row++) {
			cout << "     ";

			for (int col = 0; col < BOARD_COLUMNS; col++) {
				cout << "---";

				if (col != BOARD_COLUMNS - 1) {
					cout << " ";
				}
				else {
					cout << endl;
				}
			}

			if (row == BOARD_ROWS) {
				break;
			}

			cout << "    ";

			for (int col = 0; col < BOARD_COLUMNS; col++) {
				cout << "| " << board[row][col] << " ";

				if (col == BOARD_COLUMNS - 1) {
					cout << "|" << endl;
				}
			}
		}

		cout << endl << "<------------------>" << endl << endl;
	}

	void incrementBoardStatistics(const int row, const int col) {
		rows[row]++;
		columns[col]++;

		if (row == col) {
			primaryDiagonal++;
		}

		if (row + col == 2) {
			secondaryDiagonal++;
		}
	}

	void decreaseBoardStatistics(const int row, const int col) {
		rows[row]--;
		columns[col]--;

		if (row == col) {
			primaryDiagonal--;
		}
		
		if (row + col == 2) {
			secondaryDiagonal--;
		}
	}


	/*
	* check the rows, columns vectorsand the primaryDiagonaland secondaryDiagonal variables for a terminal 
	* state(the game has finished by either 9 turns or some of the players made 3 consecutive characters)
	*/
	int terminalTest(const int depth) {

		for (int row = 0; row < BOARD_ROWS; row++) {
			if (rows[row] == MAX_CONSECUTIVE_AI) {
				return MAX_SCORE - depth;
			}
			else if (rows[row] == MAX_CONSECUTIVE_PLAYER) {
				return depth - MAX_SCORE;
			}
		}

		for (int col = 0; col < BOARD_COLUMNS; col++) {
			if (columns[col] == MAX_CONSECUTIVE_AI) {
				return MAX_SCORE - depth;
			}
			else if (columns[col] == MAX_CONSECUTIVE_PLAYER) {
				return depth - MAX_SCORE;
			}
		}

		if (primaryDiagonal == MAX_CONSECUTIVE_AI || secondaryDiagonal == MAX_CONSECUTIVE_AI) {
			return MAX_SCORE - depth;
		}
		else if (primaryDiagonal == MAX_CONSECUTIVE_PLAYER || secondaryDiagonal == MAX_CONSECUTIVE_PLAYER) {
			return depth - MAX_SCORE;
		}

		return DRAW_SCORE;
	}

	// maximizer - uses alpha-beta pruning
	int maxValue(int alpha, int beta, int depth) {
		int currentScore = 0;

		if (depth >= MIN_DEPTH_FOR_SOLUTION) {
			currentScore = terminalTest(depth);
		}

		if (currentScore != 0) {
			return currentScore;
		}

		// if we have filled the board already - made MAX_DEPTH (in this case, it is 9) number of moves
		if (depth == MAX_DEPTH - 1) {
			return DRAW_SCORE;
		}

		int maxScore = INT_MIN;
		for (int row = 0; row < BOARD_ROWS; row++) {
			for (int col = 0; col < BOARD_COLUMNS; col++) {
				if (board[row][col] == EMPTY_CELL) {
					board[row][col] = aiCharacter;
					incrementBoardStatistics(row, col);

					maxScore = max(maxScore, minValue(alpha, beta, depth + 1));

					board[row][col] = EMPTY_CELL;
					decreaseBoardStatistics(row, col);

					/*
					* alpha-beta pruning - if we have found a value which is already bigger than or equal
					* to the min value of the path so far, then we shall only get bigger numbers
					* from the other subtrees, therefore no need to go through them
					*/
					if (maxScore >= beta) {
						return maxScore;
					}

					alpha = max(alpha, maxScore);
				}
			}
		}

		return maxScore;
	}

	// minimizer - uses alpha-beta pruning
	int minValue(int alpha, int beta, int depth) {
		int currentScore = 0;

		if (depth >= MIN_DEPTH_FOR_SOLUTION) {
			currentScore = terminalTest(depth);
		}

		if (currentScore != 0) {
			return currentScore;
		}

		if (depth == MAX_DEPTH - 1) {
			return DRAW_SCORE;
		}

		int minScore = INT_MAX;
		for (int row = 0; row < BOARD_ROWS; row++) {
			for (int col = 0; col < BOARD_COLUMNS; col++) {
				if (board[row][col] == EMPTY_CELL) {
					board[row][col] = playerCharacter;
					decreaseBoardStatistics(row, col);

					minScore = min(minScore, maxValue(alpha, beta, depth + 1));

					board[row][col] = EMPTY_CELL;
					incrementBoardStatistics(row, col);

					/*
					* alpha-beta pruning - if we have found a value which is already lower than or equal to
					* the max value of the path so far, then we shall only get lower numbers from the other
					* subtrees, therefore no need to go through them
					*/
					if (minScore <= alpha) {
						return minScore;
					}

					beta = min(beta, minScore);
				}
			}
		}

		return minScore;
	}

	void determineNextAIMove(const int& depth) {
		int maxScore = INT_MIN;
		aiMove = { -1, -1 };

		int alpha = INT_MIN;
		int beta = INT_MAX;
		for (int row = 0; row < BOARD_ROWS; row++) {
			for (int col = 0; col < BOARD_COLUMNS; col++) {
				if (board[row][col] == EMPTY_CELL) {
					board[row][col] = aiCharacter;
					incrementBoardStatistics(row, col);

					int subtreeScore = minValue(alpha, beta, depth);

					board[row][col] = EMPTY_CELL;
					decreaseBoardStatistics(row, col);

					if (maxScore < subtreeScore) {
						maxScore = subtreeScore;
						aiMove = { row, col };
					}

					alpha = max(alpha, subtreeScore);
				}
			}
		}
	}

	void makeAIMove(const int& depth) {
		if (aiMove.first == -1) {
			return;
		}

		board[aiMove.first][aiMove.second] = aiCharacter;
		incrementBoardStatistics(aiMove.first, aiMove.second);

		printBoard(depth);
	}

	bool makePlayerMove(const int row, const int col, const int& depth) {
		if (row < 0 || row >= BOARD_ROWS || col < 0 || col >= BOARD_COLUMNS) {
			return false;
		}

		if (board[row][col] != EMPTY_CELL) {
			return false;
		}

		board[row][col] = playerCharacter;
		decreaseBoardStatistics(row, col);

		printBoard(depth);
		return true;
	}

	char determineTheWinner() {
		int finalScore = terminalTest(0);
		if (finalScore < 0) {
			return PLAYER_WIN_SYMBOL;
		}
		else if (finalScore > 0) {
			return PLAYER_LOSE_SYMBOL;
		}
		else {
			return DRAW_SYMBOL;
		}
	}

public:

	TicTacToe(const bool isAIFirst = true) {
		board = vector<vector<char>>(BOARD_ROWS, vector<char>(BOARD_COLUMNS, ' '));
		rows = vector<int>(BOARD_ROWS, 0);
		columns = vector<int>(BOARD_COLUMNS, 0);
		primaryDiagonal = 0;
		secondaryDiagonal = 0;
		isPersonsTurn = !isAIFirst;

		if (isAIFirst) {
			aiCharacter = FIRST_CHARACTER;
			playerCharacter = SECOND_CHARACTER;
		}
		else {
			aiCharacter = SECOND_CHARACTER;
			playerCharacter = FIRST_CHARACTER;
		}

		aiMove = { -1, -1 };
	}

	void startGame() {
		printBoard(-1);

		for (int depth = 0; depth < MAX_DEPTH; depth++) {
			if (isPersonsTurn) {
				int inputRow = -1;
				int inputCol = -1;

				do {
					cout << "Enter move (\"[row] [col]\"): ";
					cin >> inputRow >> inputCol;
					cout << endl;
				} while (!makePlayerMove(inputRow - 1, inputCol - 1, depth));
				isPersonsTurn = false;
			}
			else {
				determineNextAIMove(depth);
				makeAIMove(depth);
				isPersonsTurn = true;
			}

			if (depth >= MIN_DEPTH_FOR_SOLUTION) {
				char result = determineTheWinner();

				if (result == PLAYER_WIN_SYMBOL) {
					cout << "Congratulations! You won! The AI could not outsmart you!" << endl;
					return;
				}
				else if (result == PLAYER_LOSE_SYMBOL) {
					cout << "You lost! Better luck next time!" << endl;
					return;
				}
				else if (depth == MAX_DEPTH - 1 && result == DRAW_SYMBOL) {
					cout << "Draw! The AI is frustrated it could not beat you!" << endl;
					return;
				}
			}
		}
	}
};

int main() {

	char userAnswer = ' ';
	cout << "Do you wish to go first? [y/n]  ";

	do {
		cin >> userAnswer;
	} while (userAnswer != 'y' && userAnswer != 'n');

	bool isAIFirst = true;
	if (userAnswer == 'y') {
		isAIFirst = false;
	}

	TicTacToe t = TicTacToe(isAIFirst);
	t.startGame();

	return 0;
}
