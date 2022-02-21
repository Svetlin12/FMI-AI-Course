package bg.uni.sofia.fmi.ai.data.entry;

import java.util.List;
import java.util.Arrays;
import java.util.stream.Collectors;

public record DataEntry(String className, List<String> attributes) {
    private static int CLASS_NAME_INDEX = 0;

    public static DataEntry of(String line) {
        String[] attributes = line.split(",");

        String className = attributes[CLASS_NAME_INDEX];
        List<String> otherAttributes = Arrays.stream(attributes)
                                        .skip(1)
                                        .collect(Collectors.toList());

        return new DataEntry(className, otherAttributes);
    }
}
