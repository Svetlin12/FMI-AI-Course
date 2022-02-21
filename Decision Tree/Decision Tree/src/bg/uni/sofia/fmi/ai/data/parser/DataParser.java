package bg.uni.sofia.fmi.ai.data.parser;

import java.util.List;
import java.nio.file.Path;
import java.io.IOException;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.Collections;
import java.util.stream.Collectors;

import bg.uni.sofia.fmi.ai.data.entry.DataEntry;
import bg.uni.sofia.fmi.ai.enums.ClassifierConstants;

public class DataParser {
    private List<DataEntry> votes;
    private final List<List<DataEntry>> sets;
    private static final String dataDirectoryName = "data";

    public DataParser(String file) {
        sets = new ArrayList<>();
        parseDataFromFile(file);
        initializeSets();
    }

    private void parseDataFromFile(String file) {
        Path pathToData = Path.of(dataDirectoryName).resolve(file);

        try (var contentReader = Files.newBufferedReader(pathToData)) {
            votes = contentReader.lines()
                    .map(DataEntry::of)
                    .collect(Collectors.toList());
        } catch (IOException e) {
            e.printStackTrace();
            throw new IllegalStateException("Unexpected exception occurred while reading from file " + file);
        }
    }

    private void initializeSets() {
        Collections.shuffle(votes);

        int dataSize = votes.size();
        int subsetSize = dataSize / ClassifierConstants.CROSS_FOLD_VALIDATION_NUMBER.getNumberConstant();

        for (int i = 0, startIndex = 0, endIndex = subsetSize; i < ClassifierConstants.CROSS_FOLD_VALIDATION_NUMBER.getNumberConstant(); i++, startIndex = endIndex, endIndex += subsetSize) {
            endIndex = Math.min(endIndex, dataSize);
            sets.add(votes.subList(startIndex, endIndex));
        }
    }

    public List<List<DataEntry>> getSets() {
        return this.sets;
    }
}
