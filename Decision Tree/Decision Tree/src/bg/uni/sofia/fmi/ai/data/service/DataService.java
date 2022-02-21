package bg.uni.sofia.fmi.ai.data.service;

import java.util.List;
import java.util.ArrayList;

import bg.uni.sofia.fmi.ai.data.entry.DataEntry;
import bg.uni.sofia.fmi.ai.data.parser.DataParser;
import bg.uni.sofia.fmi.ai.decision.tree.classifier.DecisionTree;
import bg.uni.sofia.fmi.ai.enums.ClassifierConstants;

public class DataService {
    DataParser parser;
    List<Double> accuracies;
    List<List<DataEntry>> allSets;
    double accumulatedAccuracy = 0.0;

    public DataService(String fileName) {
        parser = new DataParser(fileName);
        accuracies = new ArrayList<>();
        allSets = parser.getSets();
    }

    public void runNaiveBayesClassificationOnData() {
        for (int i = 0; i < allSets.size(); i++) {
            List<DataEntry> currentTestSet = allSets.get(i);
            allSets.remove(i);

            DecisionTree tree = new DecisionTree(allSets, currentTestSet);
            accuracies.add(tree.getAccuracy());
            accumulatedAccuracy += tree.getAccuracy();

            allSets.add(i, currentTestSet);
        }
    }

    public List<Double> getAccuraciesFromTests() {
        return List.copyOf(accuracies);
    }

    public double getAverageAccuracy() {
        return accumulatedAccuracy / ClassifierConstants.CROSS_FOLD_VALIDATION_NUMBER.getNumberConstant();
    }
}