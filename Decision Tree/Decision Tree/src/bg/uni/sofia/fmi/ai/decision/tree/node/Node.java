package bg.uni.sofia.fmi.ai.decision.tree.node;

import java.util.Map;
import java.util.List;
import java.util.HashMap;
import bg.uni.sofia.fmi.ai.data.entry.DataEntry;

public class Node {
    private static final String RECURRENCE_EVENTS = "recurrence-events";
    private boolean isLeaf;
    private int attributeIndex;
    private String value;
    private final Map<String, Node> children;
    private final List<DataEntry> data;
    private final Node parent;
    private boolean areRecurrenceEventsMore;

    public Node(Node parent, List<DataEntry> data) {
        this.isLeaf = false;
        this.children = new HashMap<>();
        this.data = data;
        this.parent = parent;
        countEvents();
    }

    private void countEvents() {
        int recurrenceEvents = 0;
        int noRecurrenceEvents = 0;
        for (DataEntry entry : data) {
            if (entry.className().equals(RECURRENCE_EVENTS)) {
                recurrenceEvents++;
            }
            else {
                noRecurrenceEvents++;
            }
        }

        if (recurrenceEvents == noRecurrenceEvents) {
            areRecurrenceEventsMore = this.parent.areRecurrenceEventsMore;
            return;
        }

        areRecurrenceEventsMore = recurrenceEvents > noRecurrenceEvents;
    }

    public void setAsLeaf() {
        this.isLeaf = true;
    }

    public void setAttributeIndex(int index) {
        this.attributeIndex = index;
    }

    public void setValue(String value) {
        this.value = value;
    }

    public boolean areRecurrenceEventsMore() {
        return this.areRecurrenceEventsMore;
    }

    public void addChild(String value, Node child) {
        children.put(value, child);
    }

    public int getAttributeIndex() {
        return this.attributeIndex;
    }

    public String getValue() {
        return this.value;
    }

    public Map<String, Node> getChildren() {
        return this.children;
    }

    public boolean isLeaf() {
        return this.isLeaf;
    }
}
