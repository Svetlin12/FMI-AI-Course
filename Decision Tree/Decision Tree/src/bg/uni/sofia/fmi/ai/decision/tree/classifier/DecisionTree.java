package bg.uni.sofia.fmi.ai.decision.tree.classifier;

import java.util.Set;
import java.util.Map;
import java.util.List;
import java.util.HashSet;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.stream.Collectors;
import bg.uni.sofia.fmi.ai.enums.Attributes;
import bg.uni.sofia.fmi.ai.data.entry.DataEntry;
import bg.uni.sofia.fmi.ai.decision.tree.node.Node;
import bg.uni.sofia.fmi.ai.enums.ClassifierConstants;

public class DecisionTree {
    private static final String RECURRENCE_EVENTS = "recurrence-events";
    private static final String NO_RECURRENCE_EVENTS = "no-recurrence-events";
    private static final int USED_FEATURES_COUNT = 9;
    private static final int MAXIMUM_DEPTH = 2;
    private static final int MINIMUM_SAMPLE_SIZE = 15;
    private static final int CLASS_NAME_INDEX = 0;
    private static final List<Attributes> attributes = List.of(Attributes.values());
    private double accuracy;

    private static double log2(double p) {
        return Math.log(p) / Math.log(2);
    }

    public DecisionTree(List<List<DataEntry>> trainData, List<DataEntry> testData) {
        List<DataEntry> allTrainData = trainData.stream()
                        .flatMap(List::stream)
                        .collect(Collectors.toList());
        Node root = buildDecisionTree(allTrainData, new HashSet<>(), 0, null);
        testClassifier(root, testData);
    }

    private void testClassifier(Node root, List<DataEntry> testData) {
        int successfullyPredictedClasses = 0;
        for (DataEntry entry : testData) {
            String prediction = predict(root, entry);
            if (prediction.equals(entry.className())) {
                successfullyPredictedClasses++;
            }
        }
        accuracy = successfullyPredictedClasses * 100.0 / testData.size();
    }

    private String predict(Node node, DataEntry entry) {
        Node parent = node;
        while (node != null && !node.isLeaf()) {
            String attributeValue = entry.attributes().get(node.getAttributeIndex());
            parent = node;
            node = node.getChildren().get(attributeValue);
        }

        if (node == null) {
            if (parent.areRecurrenceEventsMore()) {
                return RECURRENCE_EVENTS;
            }
            else {
                return NO_RECURRENCE_EVENTS;
            }
        }

        return node.getValue();
    }

    private Node buildDecisionTree(List<DataEntry> currentData, Set<Integer> usedAttributeIndices, int depth, Node parent) {
        Node node = new Node(parent, currentData);

        if (calculateEntropy(currentData) == 0) {
            node.setAsLeaf();
            node.setValue(currentData.get(0).className());
            return node;
        }

        if (currentData.size() < MINIMUM_SAMPLE_SIZE || depth == MAXIMUM_DEPTH || usedAttributeIndices.size() == USED_FEATURES_COUNT) {
            node.setAsLeaf();

            if (node.areRecurrenceEventsMore()) {
                node.setValue(RECURRENCE_EVENTS);
            }
            else {
                node.setValue(NO_RECURRENCE_EVENTS);
            }

            return node;
        }

        int maxGainIndex = getIndexWithMaxGain(currentData, usedAttributeIndices);
        node.setAttributeIndex(maxGainIndex);
        node.setValue(attributes.get(maxGainIndex).getAttributeName());

        for (String value : getUniqueValuesOfAttribute(currentData, maxGainIndex)) {
            List<DataEntry> subData = new ArrayList<>();
            for (DataEntry entry : currentData) {
                if (entry.attributes().get(maxGainIndex).equals(value)) {
                    subData.add(entry);
                }
            }

            usedAttributeIndices.add(maxGainIndex);
            Node child = buildDecisionTree(subData, usedAttributeIndices, depth + 1, node);
            node.addChild(value, child);
        }

        return node;
    }

    private Map<String, Integer> getAttributeValuesCount(List<DataEntry> data, int attributeIndex) {
        Map<String, Integer> attributeValues = new HashMap<>();
        for (DataEntry entry : data) {
            String value;
            if (attributeIndex == CLASS_NAME_INDEX) {
                value = entry.className();
            }
            else {
                value = entry.attributes().get(attributeIndex - 1);
            }

            putNewValueAt(attributeValues, value);
        }

        return attributeValues;
    }

    private void putNewValueAt(Map<String, Integer> attributeValues, String at) {
        int newValue = 1;

        if (attributeValues.containsKey(at)) {
            newValue += attributeValues.get(at);
        }

        attributeValues.put(at, newValue);
    }

    private double calculateEntropy(List<DataEntry> data) {
        double entropy = 0;

        for (Map.Entry<String, Integer> entry : getAttributeValuesCount(data, CLASS_NAME_INDEX).entrySet()) {
            double p = entry.getValue() * 1.0 / data.size();
            entropy += -1.0 * p * log2(p);
        }

        return entropy;
    }

    private double calculateEntropyForClass(List<DataEntry> data, int index) {
        double entropy = 0;
        for (Map.Entry<String, Integer> entry : getAttributeValuesCount(data, index + 1).entrySet()) {
            List<DataEntry> filteredData = new ArrayList<>();

            for (DataEntry dataEntry : data) {
                if (dataEntry.attributes().get(index).equals(entry.getKey())) {
                    filteredData.add(dataEntry);
                }
            }

            double p = entry.getValue() * 1.0 / data.size();
            entropy += p * calculateEntropy(filteredData);
        }

        return entropy;
    }

    private double calculateGain(List<DataEntry> data, int index) {
        return calculateEntropy(data) - calculateEntropyForClass(data, index);
    }

    private int getIndexWithMaxGain(List<DataEntry> data, Set<Integer> usedAttributeIndices) {
        double maxGain = Integer.MIN_VALUE;
        int maxGainIndex = Integer.MIN_VALUE;
        for (int i = 0; i < ClassifierConstants.FEATURES_COUNT.getNumberConstant() - 1; i++) {
            if (usedAttributeIndices.contains(i)) {
                continue;
            }

            double currentGain = calculateGain(data, i);
            if (currentGain > maxGain) {
                maxGain = currentGain;
                maxGainIndex = i;
            }
        }
        return maxGainIndex;
    }

    private Set<String> getUniqueValuesOfAttribute(List<DataEntry> data, int attributeIndex) {
        Set<String> uniqueValues = new HashSet<>();
        for (DataEntry entry : data) {
            uniqueValues.add(entry.attributes().get(attributeIndex));
        }
        return uniqueValues;
    }

    public double getAccuracy() {
        return this.accuracy;
    }
}