package bg.uni.sofia.fmi.ai;

import java.util.List;
import java.text.DecimalFormat;
import bg.uni.sofia.fmi.ai.data.service.DataService;

public class Main {
    private static final DecimalFormat formatPercentage = new DecimalFormat("0.00");

    public static void main(String[] args) {
        DataService service = new DataService("house-votes-84.data");
        service.runNaiveBayesClassificationOnData();

        List<Double> accuracies = service.getAccuraciesFromTests();

        for (int i = 0; i < accuracies.size(); i++) {
            System.out.println("Accuracy with test set " + i + ": " + formatPercentage.format(accuracies.get(i)));
        }

        System.out.println("Average accuracy: " + formatPercentage.format(service.getAverageAccuracy()));
    }
}
