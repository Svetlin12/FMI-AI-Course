#include <iostream>
#include <vector>
#include <random>
#include <unordered_map>
using namespace std;

const double MUTATION_PROBABILITY = 0.4; // the probability of a mutation occurring in a child
const double NEW_GENERATION_RATIO = 0.5; // the ratio of the new generation compared to the total population
const int X_BEGIN = 0; // the start of the x axis for the cities' location
const int X_END = 1000; // the end of the x axis for the cities' location
const int Y_BEGIN = 0; // the start of the y axis for the cities' location
const int Y_END = 1000; // the end of the y axis for the cities' location
const double NO_TOURNAMENT_POPULATION_RATIO = 0.05; // the number of individuals we shall keep for the next generation population
const double SELECTED_RATIO = 0.3; // the number of individuals we shall select for the breeding stage
const int INDIVIDUALS_IN_TOURNAMENT = 10; // how many individuals shall compete in a single tournament
const int MAX_NO_SIGNIFICANT_CHANGE_LOOPS = 30; // the max number of times the population will have no significant change in optimality
const double EPSILON = 0.1; // used to determine whether on the current loop there was a significant change in the population's optimality
int citiesCount;
int populationNumber;
int newGenerationCount;
int tournamentWinnersCount;
int tournamentCompetitorsCount;
int noTournamentIndividualsCount;
random_device rd; // used for the randomNumberGenerator for seeding
mt19937 randomNumberGenerator;
unordered_map<int, int> firstChildIndices;
unordered_map<int, int> secondChildIndices;
vector<int*> population;
unordered_map<int, vector<double>> distances; // key -> index of the city, value -> the distance to each of the other cities
vector<pair<int, int>> cities; // pair.first -> x coordinate of the city, pair.second -> y coordinate of the city
vector<double> fitness;

int getRandomInteger(int from, int to) {
    return uniform_int_distribution<>(from, to)(randomNumberGenerator);
}

double getRandomDouble(double from, double to) {
    return uniform_real_distribution<>(from, to)(randomNumberGenerator);
}

// go through the person and accumulate sum of the distances between each two consecutive elements
double calculateFitness(int* person) {
    double fitnessValue = 0;
    for (int i = 0; i < citiesCount - 1; i++) {
        fitnessValue += distances[person[i]][person[i + 1]];
    }
    return fitnessValue;
}

// realization of one-point crossover
pair<int*, int*> doOnePointCrossover(const int* firstParent, const int* secondParent) {
    vector<bool> hasGeneBeenSelectedFromFirstParent(citiesCount + 1, false);
    vector<bool> hasGeneBeenSelectedFromSecondParent(citiesCount + 1, false);
    int* firstChild = new int[citiesCount];
    int* secondChild = new int[citiesCount];

    for (int i = 0; i < citiesCount; i++) {
        firstChild[i] = -1;
        secondChild[i] = -1;
    }

    int pivotIndex = getRandomInteger(0, citiesCount - 1);

    for (int i = 0; i <= pivotIndex; i++) {
        firstChild[i] = firstParent[i];
        secondChild[i] = secondParent[i];
        hasGeneBeenSelectedFromFirstParent[secondParent[i]] = true;
        hasGeneBeenSelectedFromSecondParent[firstParent[i]] = true;
    }

    int firstChildContinueIndex = pivotIndex + 1;
    int secondChildContinueIndex = pivotIndex + 1;
    for (int i = 0; i < citiesCount; i++) {
        if (!hasGeneBeenSelectedFromSecondParent[secondParent[i]]) {
            hasGeneBeenSelectedFromSecondParent[secondParent[i]] = true;
            firstChild[firstChildContinueIndex++] = secondParent[i];
        }

        if (!hasGeneBeenSelectedFromFirstParent[firstParent[i]]) {
            hasGeneBeenSelectedFromFirstParent[firstParent[i]] = true;
            secondChild[secondChildContinueIndex++] = firstParent[i];
        }

        if (firstChildContinueIndex == citiesCount && secondChildContinueIndex == citiesCount) {
            break;
        }
    }

    return { firstChild, secondChild };
}

// realization of two-point crossover
pair<int*, int*> doTwoPointCrossover(const int* firstParent, const int* secondParent) {
    vector<bool> hasGeneBeenSelectedFromFirstParent(citiesCount + 1, false);
    vector<bool> hasGeneBeenSelectedFromSecondParent(citiesCount + 1, false);
    int* firstChild = new int[citiesCount];
    int* secondChild = new int[citiesCount];

    for (int i = 0; i < citiesCount; i++) {
        firstChild[i] = -1;
        secondChild[i] = -1;
    }

    int startIndex = getRandomInteger(0, citiesCount - 1);
    int endIndex = getRandomInteger(startIndex, citiesCount - 1);

    for (int i = startIndex; i <= endIndex; i++) {
        firstChild[i] = firstParent[i];
        secondChild[i] = secondParent[i];
        hasGeneBeenSelectedFromFirstParent[secondParent[i]] = true;
        hasGeneBeenSelectedFromSecondParent[firstParent[i]] = true;
    }

    int firstChildIndex = endIndex + 1;
    int secondChildIndex = endIndex + 1;

    for (int i = endIndex + 1; (firstChildIndex % citiesCount) != startIndex || (secondChildIndex % citiesCount) != startIndex; i++) {
        int realIndex = i % citiesCount;
        if (!hasGeneBeenSelectedFromSecondParent[secondParent[realIndex]]) {
            hasGeneBeenSelectedFromSecondParent[secondParent[realIndex]];
            firstChild[firstChildIndex % citiesCount] = secondParent[realIndex];
            firstChildIndex++;
        }

        if (!hasGeneBeenSelectedFromFirstParent[firstParent[realIndex]]) {
            hasGeneBeenSelectedFromFirstParent[firstParent[realIndex]];
            secondChild[secondChildIndex % citiesCount] = firstParent[realIndex];
            secondChildIndex++;
        }
    }

    return { firstChild, secondChild };
}

// realization of partially-mapped crossover
pair<int*, int*> doPartiallyMappedCrossover(const int* firstParent, const int* secondParent) {
    vector<bool> hasGeneBeenSelectedFromFirstParent(citiesCount + 1, false);
    vector<bool> hasGeneBeenSelectedFromSecondParent(citiesCount + 1, false);
    int* firstChild = new int[citiesCount];
    int* secondChild = new int[citiesCount];

    for (int i = 0; i < citiesCount; i++) {
        firstChild[i] = -1;
        secondChild[i] = -1;
    }

    for (int i = 0; i < citiesCount; i++) {
        firstChildIndices[firstParent[i]] = i;
        secondChildIndices[secondParent[i]] = i;
    }

    int startIndex = getRandomInteger(0, citiesCount - 1);
    int endIndex = getRandomInteger(startIndex, citiesCount - 1);

    for (int i = startIndex; i <= endIndex; i++) {
        firstChild[i] = firstParent[i];
        secondChild[i] = secondParent[i];
        hasGeneBeenSelectedFromFirstParent[secondParent[i]] = true;
        hasGeneBeenSelectedFromSecondParent[firstParent[i]] = true;
    }

    for (int i = startIndex; i <= endIndex; i++) {
        if (!hasGeneBeenSelectedFromSecondParent[secondParent[i]]) {
            int currentIndex = i;
            while (currentIndex >= startIndex && currentIndex <= endIndex) {
                int firstParentEquivalent = firstParent[currentIndex];
                currentIndex = secondChildIndices[firstParentEquivalent];
            }
            firstChild[currentIndex] = secondParent[i];
            hasGeneBeenSelectedFromSecondParent[secondParent[i]] = true;
        }

        if (!hasGeneBeenSelectedFromFirstParent[firstParent[i]]) {
            int currentIndex = i;
            while (currentIndex >= startIndex && currentIndex <= endIndex) {
                int secondParentEquivalent = secondParent[currentIndex];
                currentIndex = firstChildIndices[secondParentEquivalent];
            }
            secondChild[currentIndex] = firstParent[i];
            hasGeneBeenSelectedFromFirstParent[firstParent[i]] = true;
        }
    }

    for (int i = 0; i < citiesCount; i++) {
        if (!hasGeneBeenSelectedFromSecondParent[secondParent[i]]) {
            hasGeneBeenSelectedFromSecondParent[secondParent[i]] = true;
            firstChild[i] = secondParent[i];
        }

        if (!hasGeneBeenSelectedFromFirstParent[firstParent[i]]) {
            hasGeneBeenSelectedFromFirstParent[firstParent[i]] = true;
            secondChild[i] = firstParent[i];
        }
    }

    return { firstChild, secondChild };
}

// realization of cyclic crossover
pair<int*, int*> doCyclicCrossover(const int* firstParent, const int* secondParent) {
    int* firstChild = new int[citiesCount];
    int* secondChild = new int[citiesCount];

    for (int i = 0; i < citiesCount; i++) {
        firstChild[i] = -1;
        secondChild[i] = -1;
        firstChildIndices[firstParent[i]] = i;
    }

    for (int i = 0; i < citiesCount; i++) {
        int currentIndex = i;
        bool isIndexEven = (i % 2 == 0);
        while (firstChild[currentIndex] == -1) {
            if (isIndexEven) {
                firstChild[currentIndex] = firstParent[currentIndex];
                secondChild[currentIndex] = secondParent[currentIndex];
            }
            else {
                firstChild[currentIndex] = secondParent[currentIndex];
                secondChild[currentIndex] = firstParent[currentIndex];
            }
            currentIndex = firstChildIndices[secondParent[currentIndex]];
        }
    }

    return { firstChild, secondChild };
}

// switches two elements
void doSwitchMutation(int* child) {
    int firstIndex = 0;
    int secondIndex = 0;

    while (firstIndex == secondIndex) {
        firstIndex = getRandomInteger(0, citiesCount - 1);
        secondIndex = getRandomInteger(0, citiesCount - 1);
    }

    swap(child[firstIndex], child[secondIndex]);
}

// inserts a random gene from the child to a new random position in the child
void doInsertMutation(int* child) {
    int insertFrom = getRandomInteger(0, citiesCount - 2);
    int insertTo = getRandomInteger(insertFrom + 1, citiesCount - 1);

    int temp = child[insertFrom];
    for (int i = insertFrom; i < insertTo; i++) {
        child[i] = child[i + 1];
    }
    child[insertTo] = temp;
}

// reverses a sub-section of the child's chromosome
void doReverseMutation(int* child) {
    int reverseStart = getRandomInteger(0, citiesCount - 2);
    int reverseEnd = getRandomInteger(reverseStart + 1, citiesCount - 1);

    while (reverseStart < reverseEnd) {
        swap(child[reverseStart], child[reverseEnd]);
        reverseStart++;
        reverseEnd--;
    }
}

void mutate(int* child) {
    double currentProbability = getRandomDouble(0.0, 1.0);
    if (currentProbability > MUTATION_PROBABILITY) { // determine whether a mutation will occur in the current child
        return;
    }

    int mutationNumber = getRandomInteger(1, 3); // pick one of the three mutations

    if (mutationNumber == 1) {
        doReverseMutation(child);
    }
    else if (mutationNumber == 2) {
        doSwitchMutation(child);
    }
    else {
        doInsertMutation(child);
    }
}

// do a tournament - <INDIVIDUALS_IN_TOURNAMENT> individuals will compete against each other and the one with
// the lowest fitness value will win (lowest fitness -> shortest path)
int getLowestFitnessIndividual(const vector<int>& currentTournamentCompetitors, const vector<double>& tournamentCompetitorsFitness) {
    int bestIndividualIndex = currentTournamentCompetitors[0];
    double lowestFitness = tournamentCompetitorsFitness[bestIndividualIndex];
    for (int individualIndex : currentTournamentCompetitors) {
        if (tournamentCompetitorsFitness[individualIndex] < lowestFitness) {
            lowestFitness = tournamentCompetitorsFitness[individualIndex];
            bestIndividualIndex = individualIndex;
        }
    }
    return bestIndividualIndex;
}

// do <populationNumber> amount of tournaments, sort the results from those tournaments
// and based on the sorted values, pick <tournamentWinnersCount> of them and return them sorted by their fitness value
// tournamentWinnersCount - the amount of people in the population - the amount of people chosen for the next generation without tournaments
vector<int> chooseBestIndividuals(const vector<double>& tournamentCompetitorsFitness) {
    vector<pair<int, int>> tournamentResults(tournamentCompetitorsCount);
    for (int i = 0; i < tournamentCompetitorsCount; i++) {
        tournamentResults[i].first = i;
        tournamentResults[i].second = 0;
    }

    vector<int> currentTournamentCompetitors(INDIVIDUALS_IN_TOURNAMENT);
    for (int i = 0; i < populationNumber; i++) {
        for (int j = 0; j < INDIVIDUALS_IN_TOURNAMENT; j++) {
            currentTournamentCompetitors[j] = getRandomInteger(0, tournamentCompetitorsCount - 1);
        }
        int winner = getLowestFitnessIndividual(currentTournamentCompetitors, tournamentCompetitorsFitness);
        tournamentResults[winner].second++;
    }

    sort(tournamentResults.begin(), tournamentResults.end(), [&](const pair<int, int>& leftResult, const pair<int, int>& rightResult) {
        if (leftResult.second == rightResult.second) {
            return tournamentCompetitorsFitness[leftResult.first] < tournamentCompetitorsFitness[rightResult.first];
        }
        else {
            return leftResult.second > rightResult.second;
        }
    });

    vector<int> tournamentWinners(tournamentWinnersCount);
    for (int i = 0; i < tournamentWinnersCount; i++) {
        tournamentWinners[i] = tournamentResults[i].first;
    }

    sort(tournamentWinners.begin(), tournamentWinners.end(), [&](const int leftWinner, const int rightWinner) {
        return tournamentCompetitorsFitness[leftWinner] > tournamentCompetitorsFitness[rightWinner];
    });

    return tournamentWinners;
}

// determine which individuals from the previous generation and their children will participate in the next generation
void survivalStage(const vector<int*>& newGeneration) {
    vector<int*> tournamentCompetitors(tournamentCompetitorsCount);
    vector<double> tournamentCompetitorsFitness(tournamentCompetitorsCount);

    // fill all the participants in the tournaments in one place
    int i = 0;
    for (; i < tournamentWinnersCount; i++) {
        tournamentCompetitors[i] = population[i];
        tournamentCompetitorsFitness[i] = fitness[i];
    }

    for (int j = 0; i < tournamentCompetitorsCount; i++, j++) {
        tournamentCompetitors[i] = newGeneration[j];
        tournamentCompetitorsFitness[i] = calculateFitness(tournamentCompetitors[i]);
    }

    // get the winners from the tournaments
    vector<int> winners = chooseBestIndividuals(tournamentCompetitorsFitness);

    // in the new population, add the ones saved from the previous generation and the winners from the tournaments
    for (i = 0; i < tournamentWinnersCount; i++) {
        population[i] = tournamentCompetitors[winners[i]];
        fitness[i] = tournamentCompetitorsFitness[winners[i]];
        tournamentCompetitors[winners[i]] = nullptr;
    }

    for (i = tournamentWinnersCount; i < populationNumber; i++) {
        int* storage = population[i];
        double currentFitness = fitness[i];
        int left = i - 1;
        for (; left >= 0 && fitness[left] < currentFitness; left--) {
            population[left + 1] = population[left];
            fitness[left + 1] = fitness[left];
        }
        population[left + 1] = storage;
        fitness[left + 1] = currentFitness;
    }

    // clean up
    for (i = 0; i < tournamentCompetitorsCount; i++) {
        if (tournamentCompetitors[i] != nullptr) {
            delete[] tournamentCompetitors[i];
        }
    }
}

// randomly select the individuals that will be used for the breeding stage
// this function realizes stochastic universal sampling
vector<int> selection() {
    int selectedCount = (int)(SELECTED_RATIO * populationNumber); // the amount of individuals that will be selected
    double totalFitnessValue = 0; // accumulate all the fitness values here
    for (int i = 0; i < populationNumber; i++) {
        totalFitnessValue += fitness[i];
    }

    vector<double> cumulativeFitness = vector<double>(populationNumber);
    cumulativeFitness[0] = fitness[0] / totalFitnessValue;
    for (int i = 1; i < populationNumber; i++) {
        cumulativeFitness[i] = cumulativeFitness[i - 1] + fitness[i] / totalFitnessValue;
    }

    int currentSelected = 0;
    vector<int> selected;
    double randomProbability = getRandomDouble(0.0, 1.0 / selectedCount);
    int counter = 0;

    while (currentSelected < selectedCount) {
        while (randomProbability <= cumulativeFitness[counter]) {
            selected.push_back(counter);
            currentSelected++;
            randomProbability += 1.0 / selectedCount;
        }
        counter++;
    }

    return selected;
}

// the breeding stage - get the selected individuals, randomly select two of them and crossover them to make a child
vector<int*> makeNewGeneration(const vector<int>& selected) {
    vector<int*> newGeneration;
    for (int i = 0; i < newGenerationCount; i++) {
        int parentOneIndex = selected[getRandomInteger(0, selected.size() - 1)];
        int parentTwoIndex = selected[getRandomInteger(0, selected.size() - 1)];

        double randomProbability = getRandomDouble(0.0, 1.0);
        pair<int*, int*> children;
        if (randomProbability <= 0.3) { // 30% chance partially-mapped crossover will be used
            children = doPartiallyMappedCrossover(population[parentOneIndex], population[parentTwoIndex]);
        }
        else if (randomProbability <= 0.6) { // 30% chance cyclic crossover will be used
            children = doCyclicCrossover(population[parentOneIndex], population[parentTwoIndex]);
        }
        else if (randomProbability <= 0.7) { // 10% chance one point crossover will be used
            children = doOnePointCrossover(population[parentOneIndex], population[parentTwoIndex]);
        }
        else { // 30% chance two point crossover will be used
            children = doTwoPointCrossover(population[parentOneIndex], population[parentTwoIndex]);
        }

        mutate(children.first);
        mutate(children.second);

        newGeneration.push_back(children.first);
        newGeneration.push_back(children.second);
    }

    return newGeneration;
}

void calculateFitnessOfEntirePopulation() {
    sort(population.begin(), population.end(), [](int* leftChild, int* rightChild) {
        return calculateFitness(leftChild) > calculateFitness(rightChild);
    });

    for (int i = 0; i < populationNumber; i++) {
        for (int j = 0; j < citiesCount - 1; j++) {
            fitness[i] += distances[population[i][j]][population[i][j + 1]];
        }
    }
}

void calculateDistances() {
    for (int i = 0; i < citiesCount; i++) {
        for (int j = 0; j < citiesCount; j++) {
            if (i == j) {
                continue;
            }

            int xDiff = cities[i].first - cities[j].first;
            int squaredXDiff = xDiff * xDiff;
            int yDiff = cities[i].second - cities[j].second;
            int squaredYDiff = yDiff * yDiff;
            distances[i][j] = sqrt(squaredXDiff + squaredYDiff);
        }
    }
}

void createCities() {
    for (int i = 0; i < citiesCount; i++) {
        cities[i].first = getRandomInteger(X_BEGIN, X_END);
        cities[i].second = getRandomInteger(Y_BEGIN, Y_END);
        distances[i] = vector<double>(citiesCount);
    }

    calculateDistances();
}

void createPopulation() {
    int* initialPath = new int[citiesCount];
    for (int i = 0; i < citiesCount; i++) {
        initialPath[i] = i;
    }

    for (int i = 0; i < populationNumber; i++) {
        shuffle(initialPath, initialPath + citiesCount, randomNumberGenerator);
        population[i] = new int[citiesCount];
        for (int j = 0; j < citiesCount; j++) {
            population[i][j] = initialPath[j];
        }
    }
}

void printPath(const int index) {
    cout << "| ";
    for (int i = 0; i < citiesCount; i++) {
        if (i != citiesCount - 1) {
            cout << population[index][i] << "->";
        }
        else {
            cout << population[index][i] << " |";
        }
    }
    cout << endl;
}

void showStatistics() {
    double bestFitness = fitness[0];
    int bestIndex = 0;
    for (int i = 1; i < populationNumber; i++) {
        if (bestFitness > fitness[i]) {
            bestFitness = fitness[i];
            bestIndex = i;
        }
    }
    cout << "***STATISTICS***" << endl;
    cout << "Best Fitness: " << bestFitness << endl << "Corresponding Path: ";
    printPath(bestIndex);
    cout << "****************" << endl << endl;
}

void initialize() {
    cout << "Number of cities: ";
    cin >> citiesCount;

    randomNumberGenerator = mt19937(rd());
    cities = vector<pair<int, int>>(citiesCount);

    cout << "Population number: ";
    cin >> populationNumber;

    population = vector<int*>(populationNumber);
    fitness = vector<double>(populationNumber);
    noTournamentIndividualsCount = (int)(NO_TOURNAMENT_POPULATION_RATIO * populationNumber);
    newGenerationCount = (int)(NEW_GENERATION_RATIO * populationNumber);
    tournamentCompetitorsCount = populationNumber - noTournamentIndividualsCount + newGenerationCount;
    tournamentWinnersCount = populationNumber - noTournamentIndividualsCount;
}

void solveProblem() {
    int numberOfNoSignificantChangeLoops = 0;
    int currentGeneration = 0;
    while (numberOfNoSignificantChangeLoops < MAX_NO_SIGNIFICANT_CHANGE_LOOPS) {
        vector<int> selected = selection(); // selection stage
        vector<int*> newGeneration = makeNewGeneration(selected); // breeding stage
        survivalStage(newGeneration); // determine which individuals will stay for the next generation

        // print statistics of the population
        if (currentGeneration == 0 || currentGeneration % 300 == 0) {
            showStatistics();
        }

        // determine whether or not we are in a local extreme and can't get a better solution
        if (fitness[0] - fitness[populationNumber - 1] < EPSILON) {
            numberOfNoSignificantChangeLoops++;
        }
        else {
            numberOfNoSignificantChangeLoops = 0;
        }
        currentGeneration++;
    }

    showStatistics();
    cout << "Total number of generations: " << currentGeneration << endl;
}

int main() {

    initialize();
    createCities();
    createPopulation();
    calculateFitnessOfEntirePopulation();

    solveProblem();

    // clean up
    for (int i = 0; i < populationNumber; i++) {
        delete[] population[i];
    }

    return 0;
}

/*
* Custom test. Insert code in function createCities()

for (int i = 0; i < citiesCount; i++) {
    distances[i] = vector<double>(citiesCount);
}

cities[0].first = 0.190032E-03;
cities[0].second = -0.285946E-03;
cities[1].first = 383.458;
cities[1].second = -0.608756E-03;
cities[2].first = -27.0206;
cities[2].second = -282.758;
cities[3].first = 335.751;
cities[3].second = -269.577;
cities[4].first = 69.4331;
cities[4].second = -246.780;
cities[5].first = 168.521;
cities[5].second = 31.4012;
cities[6].first = 320.350;
cities[6].second = -160.900;
cities[7].first = 179.933;
cities[7].second = -318.031;
cities[8].first = 492.671;
cities[8].second = -131.563;
cities[9].first = 112.198;
cities[9].second = -110.561;
cities[10].first = 306.320;
cities[10].second = -108.090;
cities[11].first = 217.343;
cities[11].second = -447.089;
*/
