#include <iostream>
#include <fstream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <set>
#include <random>

using namespace std;

const int DATA_POINT_SIZE = 3;
const int CENTROID_POINT_SIZE = 15;
const int CENTROID_EDGE_COUNT = 4;
const int WIDTH = 1820, HEIGHT = 980;
const int SCALE_BY = 1000;
const int MAX_ITERATIONS = 100;
int maxX, maxY, minX, minY;
int scaledMaxX, scaledMaxY, scaledMinX, scaledMinY;
int rangeX, rangeY;
int K;
bool isIntraEvaluationUsed = false;

struct Point {
	double x;
	double y;

	int centroidIndex;

	Point(double x, double y) {
		this->x = x;
		this->y = y;
		this->centroidIndex = -1;
	}

	bool operator==(const Point& p) const {
		return this->x == p.x && this->y == p.y;
	}

	double squaredDistanceTo(const Point& p) const {
		return (this->x - p.x) * (this->x - p.x) + (this->y - p.y) * (this->y - p.y);
	}
};

vector<Point> dataPoints;
vector<Point> centroids;
set<int> centroidsIndices;
vector<double> distances;
vector<Point> centroidsToDraw;
vector<Point> dataPointsToDraw;

random_device dev;
mt19937 rng(dev());

int getRandomInt(int from, int to) {
	return uniform_int_distribution<mt19937::result_type>(from, to)(rng);
}

class CompareByX {
public:
	bool operator() (const Point& p1, const Point& p2) {
		return p1.x < p2.x;
	}
};

class CompareByY {
public:
	bool operator() (const Point& p1, const Point& p2) {
		return p1.y < p2.y;
	}
};

void findMaxAndMinX() {
	auto maxPoint = max_element(dataPoints.begin(), dataPoints.end(), CompareByX());
	auto minPoint = min_element(dataPoints.begin(), dataPoints.end(), CompareByX());
	Point maxPointRef = dataPoints[distance(dataPoints.begin(), maxPoint)];
	Point minPointRef = dataPoints[distance(dataPoints.begin(), minPoint)];

	maxX = maxPointRef.x;
	minX = minPointRef.x;
	rangeX = maxX - minX;
	scaledMaxX = maxPointRef.x * SCALE_BY;
	scaledMinX = minPointRef.x * SCALE_BY;
}

void findMaxAndMinY() {
	auto maxPoint = max_element(dataPoints.begin(), dataPoints.end(), CompareByY());
	auto minPoint = min_element(dataPoints.begin(), dataPoints.end(), CompareByY());
	Point maxPointRef = dataPoints[distance(dataPoints.begin(), maxPoint)];
	Point minPointRef = dataPoints[distance(dataPoints.begin(), minPoint)];

	maxY = maxPointRef.y;
	minY = minPointRef.y;
	rangeY = maxY - minY;
	scaledMaxY = maxPointRef.y * SCALE_BY;
	scaledMinY = minPointRef.y * SCALE_BY;
}

double findMinimalDistanceFromPointToCentroid(const int& pointIndex) {
	double minDistance = LLONG_MAX;
	double currDistance;

	for (const int& centroidIndex : centroidsIndices) {
		currDistance = dataPoints[pointIndex].squaredDistanceTo(dataPoints[centroidIndex]);
		minDistance = min(minDistance, currDistance);
	}

	return minDistance;
}

int chooseNewCentroid() {
	double maxDistance = 0;
	int maxDistanceIndex = 0;
	for (int i = 0; i < distances.size(); i++) {
		if (maxDistance < distances[i]) {
			maxDistanceIndex = i;
			maxDistance = distances[i];
		}
	}
	return maxDistanceIndex;
}

int findNearestCentroid(const Point& p) {
	double minDistance = p.squaredDistanceTo(centroids[0]);
	double nearestCentroidIndex = 0;
	double currDistance;

	for (int i = 1; i < centroids.size(); i++) {
		currDistance = p.squaredDistanceTo(centroids[i]);

		if (currDistance < minDistance) {
			minDistance = currDistance;
			nearestCentroidIndex = i;
		}
	}

	return nearestCentroidIndex;
}

bool shiftCentroid(const int& centroidIndex) {
	long double sumXCoords = 0, sumYCoords = 0;
	int clusteredPointsCount = 0;

	for (int i = 0; i < dataPoints.size(); i++) {
		if (dataPoints[i].centroidIndex == centroidIndex) {
			sumXCoords += dataPoints[i].x;
			sumYCoords += dataPoints[i].y;
			clusteredPointsCount++;
		}
	}

	if (clusteredPointsCount == 0) {
		return false;
	}

	Point newCentroid(sumXCoords / clusteredPointsCount, sumYCoords / clusteredPointsCount);
	Point oldCentroid = centroids[centroidIndex];

	if (oldCentroid == newCentroid) {
		return false;
	}

	centroids[centroidIndex] = newCentroid;

	return true;
}

void assignPointsToClusters() {
	for (Point& point : dataPoints) {
		point.centroidIndex = findNearestCentroid(point);
	}
}

void clusterize() {
	for (int i = 0; i < MAX_ITERATIONS; i++) {
		bool shouldContinue = false;

		assignPointsToClusters();

		for (int i = 0; i < centroids.size(); i++) {
			if (shiftCentroid(i)) {
				shouldContinue = true;
			}
		}

		if (!shouldContinue) {
			return;
		}
	}
}

void initRandomCentroids() {
	for (int i = 0; i < K; i++) {
		double currXCoord = getRandomInt(scaledMinX, scaledMaxX);
		double currYCoord = getRandomInt(scaledMinY, scaledMaxY);

		Point newCentroid(currXCoord / SCALE_BY, currYCoord / SCALE_BY);
		centroids.push_back(newCentroid);
	}
}

void initCentroidsPlusPlus() {
	int firstCentroidIndex = getRandomInt(0, dataPoints.size() - 1);
	centroidsIndices.insert(firstCentroidIndex);

	int centroidsCount = 1;
	int newCentroidIndex;

	for (int centroidsCount = 1; centroidsCount < K; centroidsCount++) {
		for (int i = 0; i < dataPoints.size(); i++) {
			distances[i] = findMinimalDistanceFromPointToCentroid(i);
		}

		centroidsIndices.insert(chooseNewCentroid());
	}

	for (const int& pointIndex : centroidsIndices) {
		Point centroid(dataPoints[pointIndex].x, dataPoints[pointIndex].y);
		centroids.push_back(centroid);
	}
}

double convertXCoordinate(double coord) {
	return (coord - minX) * WIDTH / rangeX;
}

double convertYCoordinate(double coord) {
	return (maxY - coord) * HEIGHT / rangeY;
}

sf::Color getColor(int index)
{
	switch (index)
	{
	case 0:
		return sf::Color(255, 51, 51);
	case 1:
		return sf::Color(26, 117, 255);
	case 2:
		return sf::Color(0, 230, 0);
	case 3:
		return sf::Color(255, 153, 51);
	case 4:
		return sf::Color(230, 230, 0);
	case 5:
		return sf::Color(128, 255, 255);
	case 6:
		return sf::Color(179, 119, 0);
	case 7:
		return  sf::Color(210, 77, 255);
	default:
		return  sf::Color(166, 166, 166);
	}
}

void drawKMeans(string title) {
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), title);

	window.setPosition(sf::Vector2i(50, 0));

	vector<sf::CircleShape> circles;
	double x, y, index;
	for (int i = 0; i < dataPoints.size(); i++) {
		x = convertXCoordinate(dataPoints[i].x);
		y = convertYCoordinate(dataPoints[i].y);
		index = dataPointsToDraw[i].centroidIndex;
		sf::CircleShape circle(DATA_POINT_SIZE);
		circle.setFillColor(getColor(index));
		circle.setPosition(x, y);
		circles.push_back(circle);
	}

	vector<sf::CircleShape> squares;
	for (int i = 0; i < centroidsToDraw.size(); i++) {
		x = convertXCoordinate(centroidsToDraw[i].x);
		y = convertYCoordinate(centroidsToDraw[i].y);

		sf::CircleShape square(CENTROID_POINT_SIZE, CENTROID_EDGE_COUNT);
		square.setFillColor(getColor(i));
		square.setPosition(x, y);
		squares.push_back(square);
	}

	sf::Event event;

	while (window.isOpen()) {
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		window.clear(sf::Color::White);

		for (int i = 0; i < circles.size(); i++) {
			window.draw(circles[i]);
		}

		for (int i = 0; i < squares.size(); i++) {
			window.draw(squares[i]);
		}

		window.display();
	}
}

double computeIntraClusterDistanceSum() {
	double distanceSum = 0;
	for (const Point& dataPoint : dataPoints) {
		distanceSum += dataPoint.squaredDistanceTo(centroids[dataPoint.centroidIndex]);
	}
	return distanceSum;
}

double computeInterClusterDistanceSumOfCentroid(const int& centroidIndex) {
	double sum = 0;
	for (int i = 0; i < K; i++) {
		if (i == centroidIndex) {
			continue;
		}

		sum += centroids[i].squaredDistanceTo(centroids[centroidIndex]);
	}
	return sum;
}

double computeInterClusterDistanceSum() {
	double distanceSum = 0;
	for (int i = 0; i < K; i++) {
		distanceSum += computeInterClusterDistanceSumOfCentroid(i);
	}
	return distanceSum;
}

void saveCurrentState(const double& currEvaluation, double& bestEvaluation) {
	bestEvaluation = currEvaluation;
	centroidsToDraw = centroids;
	dataPointsToDraw = dataPoints;
}

void evaluateSolution(double& bestEvaluation) {
	if (isIntraEvaluationUsed) {
		double currEvaluation = computeIntraClusterDistanceSum();
		if (bestEvaluation == -1 || currEvaluation < bestEvaluation) {
			saveCurrentState(currEvaluation, bestEvaluation);
		}
	}
	else {
		double currEvaluation = computeInterClusterDistanceSum();
		if (bestEvaluation == -1 || currEvaluation > bestEvaluation) {
			saveCurrentState(currEvaluation, bestEvaluation);
		}
	}
}

void startKMeans() {
	double bestEvaluation = -1;
	for (int i = 0; i < MAX_ITERATIONS / 2; i++) {
		initRandomCentroids();
		clusterize();
		evaluateSolution(bestEvaluation);
		centroids.clear();
	}
	drawKMeans("Clustering using standard kMeans algorithm");
}

void startKMeansPlusPlus() {
	double bestEvaluation = -1;
	for (int i = 0; i < MAX_ITERATIONS / 2; i++) {
		initCentroidsPlusPlus();
		clusterize();
		evaluateSolution(bestEvaluation);
		centroidsIndices.clear();
		centroids.clear();
	}
	drawKMeans("Clustering using kMeans with improved initialization called kMeans++");
}

void startAlgorithms() {
	startKMeans();
	startKMeansPlusPlus();
}

int main() {
	string fileName, evaluationTechnique;

	cout << "K: ";
	cin >> K;

	cout << "File: ";
	cin >> fileName;

	cout << "Evaluation Technique [intra/inter]: ";
	cin >> evaluationTechnique;

	if (evaluationTechnique == "intra") {
		isIntraEvaluationUsed = true;
	}

	ifstream file(fileName);

	string line;
	double x, y;
	while (getline(file, line))
	{
		stringstream stream(line);

		stream << line;
		stream >> x >> y;

		Point p(x, y);

		dataPoints.push_back(p);
	}

	file.close();

	findMaxAndMinX();
	findMaxAndMinY();
	distances.resize(dataPoints.size());
	startAlgorithms();
}