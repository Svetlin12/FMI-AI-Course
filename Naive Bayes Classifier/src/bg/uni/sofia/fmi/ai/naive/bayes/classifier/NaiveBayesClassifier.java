package bg.uni.sofia.fmi.ai.naive.bayes.classifier;

import java.util.Map;
import java.util.List;
import java.util.EnumMap;
import java.util.ArrayList;
import bg.uni.sofia.fmi.ai.vote.Vote;
import bg.uni.sofia.fmi.ai.enums.ProbabilityType;
import bg.uni.sofia.fmi.ai.enums.ClassifierConstants;

public class NaiveBayesClassifier {
    private final List<List<Vote>> trainingData;
    private final List<Vote> testData;
    private final List<Map<ProbabilityType, Double>> trainedData;
    private double democratProbability;
    private double republicanProbability;
    private int successfullyPredictedClasses;

    public NaiveBayesClassifier(List<List<Vote>> trainingData, List<Vote> testData) {
        this.trainingData = trainingData;
        this.testData = testData;
        trainedData = new ArrayList<>();
        for (int i = 0; i < ClassifierConstants.FEATURES_COUNT.getNumberConstant(); i++) {
            trainedData.add(new EnumMap<>(ProbabilityType.class));
        }
        train();
        successfullyPredictedClasses = 0;
        testTrainedData();
    }

    private void train() {
        int numberOfDemocrats = 0;
        int numberOfRepublicans = 0;
        Map<ProbabilityType, Integer> probabilityTable = new EnumMap<>(ProbabilityType.class);

        for (int i = 0; i < ClassifierConstants.FEATURES_COUNT.getNumberConstant(); i++) {
            initializeProbabilityTable(probabilityTable);

            for (List<Vote> votes : trainingData) {
                for (Vote vote : votes) {
                    if (vote.name().equals("democrat")) {
                        numberOfDemocrats++;
                    }
                    else {
                        numberOfRepublicans++;
                    }

                    if (vote.getVoteAt(i) == null) {
                        continue;
                    }

                    countVote(probabilityTable, vote, i);
                }

                int democratsVotedYes = probabilityTable.get(ProbabilityType.DEMOCRAT_VOTE_YES);
                int democratsVotedNo = probabilityTable.get(ProbabilityType.DEMOCRAT_VOTE_NO);
                int republicansVotedYes = probabilityTable.get(ProbabilityType.REPUBLICAN_VOTE_YES);
                int republicansVotedNo = probabilityTable.get(ProbabilityType.REPUBLICAN_VOTE_NO);

                trainedData.get(i).put(ProbabilityType.DEMOCRAT_VOTE_YES, democratsVotedYes * 1.0 / (democratsVotedYes + democratsVotedNo));
                trainedData.get(i).put(ProbabilityType.DEMOCRAT_VOTE_NO, democratsVotedNo * 1.0 / (democratsVotedYes + democratsVotedNo));
                trainedData.get(i).put(ProbabilityType.REPUBLICAN_VOTE_YES, republicansVotedYes * 1.0 / (republicansVotedYes + republicansVotedNo));
                trainedData.get(i).put(ProbabilityType.REPUBLICAN_VOTE_NO, republicansVotedNo * 1.0 / (republicansVotedYes + republicansVotedNo));
            }
        }

        democratProbability = numberOfDemocrats * 1.0 / (numberOfDemocrats + numberOfRepublicans);
        republicanProbability = numberOfRepublicans * 1.0 / (numberOfDemocrats + numberOfRepublicans);
    }

    private void countVote(Map<ProbabilityType, Integer> probabilityTable, Vote vote, int index) {
        ProbabilityType type;
        if (vote.name().equals("democrat")) {
            if (vote.getVoteAt(index)) {
                type = ProbabilityType.DEMOCRAT_VOTE_YES;
            }
            else {
                type = ProbabilityType.DEMOCRAT_VOTE_NO;
            }
        }
        else {
            if (vote.getVoteAt(index)) {
                type = ProbabilityType.REPUBLICAN_VOTE_YES;
            }
            else {
                type = ProbabilityType.REPUBLICAN_VOTE_NO;
            }
        }

        int newNumberOfVotes = probabilityTable.get(type) + 1;
        probabilityTable.put(type, newNumberOfVotes);
    }

    private void initializeProbabilityTable(Map<ProbabilityType, Integer> probabilityTable) {
        probabilityTable.put(ProbabilityType.DEMOCRAT_VOTE_YES, 1);
        probabilityTable.put(ProbabilityType.DEMOCRAT_VOTE_NO, 1);
        probabilityTable.put(ProbabilityType.REPUBLICAN_VOTE_YES, 1);
        probabilityTable.put(ProbabilityType.REPUBLICAN_VOTE_NO, 1);
    }

    private void testTrainedData() {
        for (Vote vote : testData) {
            double democratLogarithmProbability = Math.log(democratProbability);
            double republicanLogarithmProbability = Math.log(republicanProbability);

            for (int i = 0; i < ClassifierConstants.FEATURES_COUNT.getNumberConstant(); i++) {
                if (vote.getVoteAt(i) == null) {
                    continue;
                }

                if (vote.getVoteAt(i)) {
                    democratLogarithmProbability += Math.log(trainedData.get(i).get(ProbabilityType.DEMOCRAT_VOTE_YES));
                    republicanLogarithmProbability += Math.log(trainedData.get(i).get(ProbabilityType.REPUBLICAN_VOTE_YES));
                }
                else {
                    democratLogarithmProbability += Math.log(trainedData.get(i).get(ProbabilityType.DEMOCRAT_VOTE_NO));
                    republicanLogarithmProbability += Math.log(trainedData.get(i).get(ProbabilityType.REPUBLICAN_VOTE_NO));
                }
            }

            if (isPredictionCorrect(democratLogarithmProbability, republicanLogarithmProbability, vote)) {
                successfullyPredictedClasses++;
            }
        }
    }

    private boolean isPredictionCorrect(double democratLogarithmProbability, double republicanLogarithmProbability, Vote vote) {
        String predicted;
        if (democratLogarithmProbability > republicanLogarithmProbability) {
            predicted = "democrat";
        }
        else {
            predicted = "republican";
        }

        return vote.name().equals(predicted);
    }

    public double getAccuracy() {
        return successfullyPredictedClasses * 100.0 / testData.size();
    }
}
