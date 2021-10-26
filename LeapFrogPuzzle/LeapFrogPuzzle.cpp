#include <iostream>
#include <vector>
using namespace std;

bool canMoveRightBy1(const vector<char>& positions, const int& posOfStone) {
    if (posOfStone - 1 < 0) { // out of bounds
        return false;
    }

    if (positions[posOfStone - 1] != 'L') { // if there is no frog that can move right next to the stone
        return false;
    }

    return true; // corner cases checked so return true
}

bool canMoveLeftBy1(const vector<char>& positions, const int& posOfStone) {
    if (posOfStone + 1 > positions.size()) { // out of bounds
        return false;
    }

    if (positions[posOfStone + 1] != 'R') { // if there is no frog that can move left next to the stone
        return false;
    }

    return true; // corner cases checked so return true
}

bool canMoveRightBy2(const vector<char>& positions, const int& posOfStone) {
    if (posOfStone - 2 < 0) {
        return false;
    }

    if (positions[posOfStone - 2] != 'L') {
        return false;
    }

    return true;
}

bool canMoveLeftBy2(const vector<char>& positions, const int& posOfStone) {
    if (posOfStone + 2 > positions.size()) { // out of bounds
        return false;
    }

    if (positions[posOfStone + 2] != 'R') {
        return false;
    }

    return true;
}

// checks whether the frogs have positioned themselves in the correct order
// i.e. for N = 3, they should be positioned: RRR_LLL
bool isFinalStateReached(vector<char>& positions) {
    // first, traverse the positions up until the stone - the '_' character
    unsigned int firstHalf = (positions.size() - 1) / 2;
    for (unsigned int i = 0; i < firstHalf; i++) {
        if (positions[i] == 'L') {
            return false;
        }
    }

    // then, traverse the positions from the stone to the end
    for (unsigned int i = firstHalf + 1; i < positions.size(); i++) {
        if (positions[i] == 'R') {
            return false;
        }
    }

    return true;
}

/* Whenever we find a solution, we have to print the path up until that solution.
 * But saving the current path in a stack takes up space which we can save.
 * Since the whole path to the solution is symmetrical, we can just print the current state backwards and
 * this will produce the state from the beginning which is symmetrical to the current one.
 * */
void printVectorBackwards(vector<char>& positions) {
    for (int i = positions.size() - 1; i >= 0; i--) {
        cout << positions[i];
    }
    cout << endl;
}

/* this function is essentially an online backtracking, meaning that on each step of the recursion, the algorithm creates
 * its state without needing a constructed tree beforehand
 * */
bool findSolution(vector<char>& positions, int posOfStone, int& N) {
    if (posOfStone == N && isFinalStateReached(positions)) { // check if we have found a solution
        printVectorBackwards(positions);
        return true;
    }

    if (canMoveLeftBy1(positions, posOfStone)) { // check if on the right of the stone there is a 'R' frog which can jump on it
        swap(positions[posOfStone], positions[posOfStone + 1]); // create the current state (the 'R' frog has jumped on the stone)
        // go further down the path with the current path
        if (findSolution(positions, posOfStone + 1, N)) { // if we have found a solution
            swap(positions[posOfStone], positions[posOfStone + 1]); // swap back to the previous state
            printVectorBackwards(positions); // print the symmetrical state
            return true;
        }
        swap(positions[posOfStone], positions[posOfStone + 1]); // swap back to the previous state
    }

    if (canMoveLeftBy2(positions, posOfStone)) { // check if on the right of the stone there is a 'R' frog two positions away from it which can jump on it
        swap(positions[posOfStone], positions[posOfStone + 2]); // create the current state (the 'R' frog has jumped on the stone)
        // go further down the path with the current path
        if (findSolution(positions, posOfStone + 2, N)) { // if we have found a solution
            swap(positions[posOfStone], positions[posOfStone + 2]); // swap back to the previous state
            printVectorBackwards(positions); // print the symmetrical state
            return true;
        }
        swap(positions[posOfStone], positions[posOfStone + 2]); // swap back to the previous state
    }

    // the other two checks are symmetrical to the previous two which we explained - only now we check to the left of the stone

    if (canMoveRightBy1(positions, posOfStone)) {
        swap(positions[posOfStone], positions[posOfStone - 1]);
        if (findSolution(positions, posOfStone - 1, N)) {
            swap(positions[posOfStone], positions[posOfStone - 1]);
            printVectorBackwards(positions);
            return true;
        }
        swap(positions[posOfStone], positions[posOfStone - 1]);
    }

    if (canMoveRightBy2(positions, posOfStone)) {
        swap(positions[posOfStone], positions[posOfStone - 2]);
        if (findSolution(positions, posOfStone - 2, N)) {
            swap(positions[posOfStone], positions[posOfStone - 2]);
            printVectorBackwards(positions);
            return true;
        }
        swap(positions[posOfStone], positions[posOfStone - 2]);
    }

    return false;
}

void initialise(int& N, vector<char>& positions) {
    cout << "N: ";
    cin >> N;

    // build the array which will represent the frogs and the stone
    positions.resize(2 * N + 1);
    for (int i = 0; i < 2 * N + 1; i++) {
        if (i == N) {
            positions[i] = '_';
        }
        else if (i < N) {
            positions[i] = 'L';
        }
        else {
            positions[i] = 'R';
        }
    }
}

int main() {
    // make cin, cout faster
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    vector<char> positions;
    initialise(N, positions);
    findSolution(positions, N, N);

    return 0;
}
