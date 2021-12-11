package bg.uni.sofia.fmi.ai.data.service;

import java.util.List;
import java.util.ArrayList;
import bg.uni.sofia.fmi.ai.vote.Vote;
import bg.uni.sofia.fmi.ai.data.parser.DataParser;
import bg.uni.sofia.fmi.ai.enums.ClassifierConstants;
import bg.uni.sofia.fmi.ai.naive.bayes.classifier.NaiveBayesClassifier;

public class DataService {
    DataParser parser;
    List<Double> accuracies;
    List<List<Vote>> allSets;
    double accumulatedAccuracy = 0.0;

    public DataService(String fileName) {
        parser = new DataParser(fileName);
        accuracies = new ArrayList<>();
        allSets = parser.getSets();
    }

    public void runNaiveBayesClassificationOnData() {
        for (int i = 0; i < allSets.size(); i++) {
            List<Vote> currentTestSet = allSets.get(i);
            allSets.remove(i);
            NaiveBayesClassifier classifier = new NaiveBayesClassifier(allSets, currentTestSet);

            accuracies.add(classifier.getAccuracy());
            accumulatedAccuracy += classifier.getAccuracy();
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