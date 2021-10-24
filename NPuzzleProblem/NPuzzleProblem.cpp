#include <iostream>
#include <vector>
#include <stack>
#include <cmath>
#include <unordered_map>
#include <algorithm>
#include <chrono>
using namespace std;

// this enum will represent the movement of the tiles
enum Movement {
    up, down, left, right, none
};

/*
 * tileCount - received as input - the number of tiles excluding the 0 (empty) tile
 * zeroPosition - the position of the empty tile in the goal state
 * sizeOfBoard - the dimension of the puzzle board (3x3, 4x4, etc...)
 */
int tileCount, zeroPosition, sizeOfBoard;

// 2d vector array representing the initial puzzle board
vector<vector<int>> board;

// for each tile number, there is a pair, representing its target row and column in the goal state
unordered_map<int, pair<int, int>> targetPositions;

// map that stores for each Movement a pair, representing the actual movement that is made (used when updating the previous position of the empty tile)
unordered_map<Movement, pair<int, int>> moves({{ Movement::up, {-1, 0}},
                                             { Movement::down, {1, 0}},
                                             { Movement::right, {0, 1}},
                                             { Movement::left, {0, -1}},
                                             { Movement::none , {0, 0}}});

// two pairs, representing the current and previous position of the empty tile
pair<int, int> currentPositionOfZero, previousPositionOfZero;

// we shall store the path to the goal state here
stack<Movement> solution;

/**
 * Predefines the operator + for two pairs. Used when updating the previous position of the empty tile.
 *
 * @param p1 the first pair
 * @param p2 the second pair
 * @return a pair representing the sum of the two pairs
 */
pair<int, int> operator+(const pair<int, int>& p1, const pair<int, int>& p2) {
    return {p1.first + p2.first, p1.second + p2.second};
}

/**
 * Calculate the size of the board by using the tile count given as input
 */
void calcSizeOfBoard() {
    sizeOfBoard = (int) sqrt(tileCount + 1);
}

/**
 * Determine the target position of each tile. Each target position depends on the target position of the empty tile.
 */
void setupTargetPositions() {
    int rowOfZero, colOfZero;

    // if the empty tile's position is to be the last one, then set up its target position as such
    if (zeroPosition == -1) {
        rowOfZero = tileCount / sizeOfBoard;
        colOfZero = tileCount - rowOfZero * sizeOfBoard;
    }
    // if the empty tile's position is to be somewhere else, then set up its target position to the given one
    else {
        rowOfZero = (zeroPosition - 1) / sizeOfBoard;
        colOfZero = (zeroPosition - 1) - rowOfZero * sizeOfBoard;
    }

    targetPositions[0] = pair<int, int>{rowOfZero, colOfZero};

    // go through each tile and evaluate its target position
    for (int i = 0, j = 1; i < tileCount + 1; i++) {
        if (i == zeroPosition - 1) {
            continue;
        }

        int row = i / sizeOfBoard;
        int col = i - row * sizeOfBoard;
        targetPositions[j++] = pair<int, int>{row, col};
    }
}

/**
 * Evaluates the Manhattan distance of a concrete tile.
 *
 * @param actualRow the row of the tile in the starting board
 * @param actualCol the column of the tile in the starting board
 * @param targetRow the row of the tile in the goal board
 * @param targetCol the column of the tile in the goal board
 * @return the Manhattan distance of the concrete tile
 */
int calcManhattanDistance(const int& actualRow, const int& actualCol, const int& targetRow, const int& targetCol) {
    return abs(actualRow - targetRow) + abs(actualCol - targetCol);
}

/**
 * Evaluates the total Manhattan distance of the puzzle board
 *
 * @return the total Manhattan distance of the puzzle board
 */
int getTotalManhattanDistance() {
    int sum = 0;
    for (int i = 0; i < sizeOfBoard; i++) {
        for (int j = 0; j < sizeOfBoard; j++) {
            if (board[i][j] == 0) {
                continue;
            }
            int targetRow = targetPositions[board[i][j]].first;
            int targetCol = targetPositions[board[i][j]].second;
            sum += calcManhattanDistance(i, j, targetRow, targetCol);
        }
    }
    return sum;
}

/**
 * The function takes the value of the Manhattan distance for the previous state and updates it so that it may represent the Manhattan distance for the
 * current state. Since only one tile (that is used in the calculation of the Manhattan distance) changed it's position, we only need to subtract its
 * previous Manhattan distance and add its new one.
 *
 * @param previousManhattanValue the estimated value of the Manhattan distance for the previous state
 * @return the estimated value of the Manhattan distance for the current state
 */
int updateTotalManhattanDistance(const int& previousManhattanValue) {
    // determine which tile number moved to the previous empty tile's position
    int numberOnPrevZeroTile = board[previousPositionOfZero.first][previousPositionOfZero.second];

    // get the above-mentioned tile number's target position on the board
    int targetRow = targetPositions[numberOnPrevZeroTile].first; // get its target row
    int targetCol = targetPositions[numberOnPrevZeroTile].second; // get its target column

    return previousManhattanValue - abs(currentPositionOfZero.first - targetRow) - abs(currentPositionOfZero.second - targetCol) +
    abs(previousPositionOfZero.first - targetRow) + abs(previousPositionOfZero.second - targetCol);
}

/**
 * Determines if the empty tile can move up.
 *
 * @return boolean, indicating whether the empty tile can move up
 */
bool canMoveUp() {
    return currentPositionOfZero.first - 1 >= 0;
}

/**
 * Determines if the empty tile can move down.
 *
 * @return boolean, indicating whether the empty tile can move down
 */
bool canMoveDown() {
    return currentPositionOfZero.first + 1 < sizeOfBoard;
}

/**
 * Determines if the empty tile can move right.
 *
 * @return boolean, indicating whether the empty tile can move right
 */
bool canMoveRight() {
    return currentPositionOfZero.second + 1 < sizeOfBoard;
}

/**
 * Determines if the empty tile can move left.
 *
 * @return boolean, indicating whether the empty tile can move left
 */
bool canMoveLeft() {
    return currentPositionOfZero.second - 1 >= 0;
}

/**
 * Receives the move that the empty tile made and returns the move that one of the adjacent tiles (depending on the concrete move) made
 *
 * @param previousMove the move which was made to reach the current state - this is essentially the move of the empty tile
 * @return the opposite move, meaning that it is the move that another tile made to get to the position of the empty tile
 */
Movement getOppositeMove(const Movement& previousMove) {
    if (previousMove == Movement::up) {
        return Movement::down; // if the empty tile made a move up, then the tile that is above the empty one made a move down
    }
    else if (previousMove == Movement::down) {
        return Movement::up; // if the empty tile made a move down, then the tile that is below the empty one made a move up
    }
    else if (previousMove == Movement::right) {
        return Movement::left; // if the empty tile made a move right, then the tile to the right of the empty one made a move to the left
    }
    else if (previousMove == Movement::left) {
        return Movement::right; // if the empty tile made a move left, then the tile to the left of the empty one made a move to the right
    }
    else return Movement::none; // only used at the start of each search - the empty tile did not move anywhere
}

/**
 * Performs the A* search with a given threshold. This means that the A* search will only get to a certain depth.
 *
 * @param accumulatedCost the number of steps needed to reach to the current state
 * @param threshold the upper bound of the current iteration of the iterative deepening A* search (we stop going further on through the states if
 *                  the cost of the current node is more than this upper bound)
 * @param previousMove the move that was made in order to reach the current state
 * @param previousManhattanValue the Manhattan value of the previous state which we shall use to calculate the Manhattan value of the current one
 * @return either 0, meaning that a goal state was reached, or the minimal cost of the child states (this cost is >= threshold) -
 *         we use this value to update the threshold
 */
int searchWithThreshold(const int accumulatedCost, const int& threshold, const Movement previousMove, const int& previousManhattanValue) {
    Movement oppositeMove = getOppositeMove(previousMove); // get the opposite move of the move that was made to get to the current state
    // based on the move that was made to get to the current state, determine the previous position of the empty tile (move it to the opposite direction)
    previousPositionOfZero = currentPositionOfZero + moves[oppositeMove];
    // evaluate the Manhattan distance of the current state based on the Manhattan value of the previous state
    int manhattanDistance = updateTotalManhattanDistance(previousManhattanValue);
    // evaluate the total cost of the current state (the cost so far to reach the current state + the estimated cost to the goal)
    int currentCost = accumulatedCost + manhattanDistance;

    // if the cost is more than the threshold, then return that cost - stops the search in the current branch of the state tree
    if (currentCost > threshold) {
        return currentCost;
    }
    // if goal state is reached
    else if (manhattanDistance == 0) {
        solution.push(oppositeMove);
        cout << "The minimum steps required to solve this puzzle are: " << endl << accumulatedCost << endl;
        return 0;
    }

    // minCost is used to determine the minimum cost that is bigger than the threshold from the child states, cost is used to store the cost returned by the child states
    int minCost = INT_MAX, cost;

    // if we can move up from the current state
    if (oppositeMove != Movement::up && canMoveUp()) {
        // swap the empty tile with the one above and update the empty tile's current position
        swap(board[currentPositionOfZero.first][currentPositionOfZero.second], board[currentPositionOfZero.first - 1][currentPositionOfZero.second]);
        currentPositionOfZero.first -= 1;

        cost = searchWithThreshold(accumulatedCost + 1, threshold, Movement::up, manhattanDistance);

        // revert the changes that we made before descending into the child state
        currentPositionOfZero.first += 1;
        swap(board[currentPositionOfZero.first][currentPositionOfZero.second], board[currentPositionOfZero.first - 1][currentPositionOfZero.second]);

        // if a goal state was reached
        if (cost == 0) {
            solution.push(oppositeMove);
            return 0;
        }

        // otherwise, determine if the minimal cost is to be updated with a better value
        minCost = min(minCost, cost);
    }

    // if we can move down from the current state
    if (oppositeMove != Movement::down && canMoveDown()) {
        // swap the empty tile with the one below and update the empty tile's current position
        swap(board[currentPositionOfZero.first][currentPositionOfZero.second], board[currentPositionOfZero.first + 1][currentPositionOfZero.second]);
        currentPositionOfZero.first += 1;

        cost = searchWithThreshold(accumulatedCost + 1, threshold, Movement::down, manhattanDistance);

        // revert the changes that we made before descending into the child state
        currentPositionOfZero.first -= 1;
        swap(board[currentPositionOfZero.first][currentPositionOfZero.second], board[currentPositionOfZero.first + 1][currentPositionOfZero.second]);

        // if a goal state was reached
        if (cost == 0) {
            solution.push(oppositeMove);
            return 0;
        }

        // otherwise, determine if the minimal cost is to be updated with a better value
        minCost = min(minCost, cost);
    }

    // if we can move right from the current state
    if (oppositeMove != Movement::right && canMoveRight()) {
        // swap the empty tile with the one to the right and update the empty tile's current position
        swap(board[currentPositionOfZero.first][currentPositionOfZero.second], board[currentPositionOfZero.first][currentPositionOfZero.second + 1]);
        currentPositionOfZero.second += 1;

        cost = searchWithThreshold(accumulatedCost + 1, threshold, Movement::right, manhattanDistance);

        // revert the changes that we made before descending into the child state
        currentPositionOfZero.second -= 1;
        swap(board[currentPositionOfZero.first][currentPositionOfZero.second], board[currentPositionOfZero.first][currentPositionOfZero.second + 1]);

        // if a goal state was reached
        if (cost == 0) {
            solution.push(oppositeMove);
            return 0;
        }

        // otherwise, determine if the minimal cost is to be updated with a better value
        minCost = min(minCost, cost);
    }

    // if we can move left from the current state
    if (oppositeMove != Movement::left && canMoveLeft()) {
        // swap the empty tile with the one to the left and update the empty tile's current position
        swap(board[currentPositionOfZero.first][currentPositionOfZero.second], board[currentPositionOfZero.first][currentPositionOfZero.second - 1]);
        currentPositionOfZero.second -= 1;

        cost = searchWithThreshold(accumulatedCost + 1, threshold, Movement::left, manhattanDistance);

        // revert the changes that we made before descending into the child state
        currentPositionOfZero.second += 1;
        swap(board[currentPositionOfZero.first][currentPositionOfZero.second], board[currentPositionOfZero.first][currentPositionOfZero.second - 1]);

        // if a goal state was reached
        if (cost == 0) {
            solution.push(oppositeMove);
            return 0;
        }

        // otherwise, determine if the minimal cost is to be updated with a better value
        minCost = min(minCost, cost);
    }

    return minCost; // return the minimal cost that is bigger than the threshold, since no goal state was reached (this minimal cost may be used as the next value for the threshold)
}

/**
 * Realises the Iterative Deepening A* search. The initial value for the threshold is the Manhattan distance of the starting state. This means that
 * the algorithm will start from the first state and stop at its child states. The minimum from their costs will be used as the next threshold value and so on
 * until we find a goal state.
 */
void solveNPuzzle() {
    int initialManhattanDistance = getTotalManhattanDistance();
    int threshold = initialManhattanDistance;
    while (true) {
        threshold = searchWithThreshold(0, threshold, Movement::none, initialManhattanDistance);

        // if goal state was reached
        if (threshold == 0) {
            return;
        }
    }
}

/**
 * Convert the enum value from Movement to a string value.
 *
 * @param move a move stored in the solution stack
 * @return string representing the move
 */
const string getMovementAsString(const Movement& move) {
    switch (move) {
        case Movement::up:
            return "up";
        case Movement::down:
            return "down";
        case Movement::left:
            return "left";
        case Movement::right:
            return "right";
        default:
            return "none";
    }
}

int main() {
    cout << "Enter the count of the tile count: ";
    cin >> tileCount;

    cout << "Enter the finishing position of the zero: ";
    cin >> zeroPosition;

    if (zeroPosition > tileCount || (zeroPosition != -1 && zeroPosition <= 0)) {
        cout << "Please enter a valid position for the zero:\n1. -1 for the bottom right corner;\n2. in the range [1, tile count].";
        return 0;
    }

    calcSizeOfBoard();

    cout << "Enter the puzzle: " << endl;

    board.resize(sizeOfBoard);
    for (int i = 0; i < sizeOfBoard; i++) {
        board[i].resize(sizeOfBoard);
        for (int j = 0; j < sizeOfBoard; j++) {
            cin >> board[i][j];
            if (board[i][j] == 0) {
                currentPositionOfZero = pair<int, int>{i, j};
            }
        }
    }

    setupTargetPositions();

    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    solveNPuzzle();
    chrono::steady_clock::time_point end = chrono::steady_clock::now();

    cout << "The steps required to solve the puzzle are as follows:" << endl;

    while (!solution.empty()) {
        if (solution.top() != Movement::none) {
            cout << getMovementAsString(solution.top()) << endl;
        }
        solution.pop();
    }

    cout << "runtime (in milliseconds): " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << endl;
    cout << "runtime (in seconds): " << chrono::duration_cast<chrono::seconds >(end - begin).count() << endl;

    return 0;
}
