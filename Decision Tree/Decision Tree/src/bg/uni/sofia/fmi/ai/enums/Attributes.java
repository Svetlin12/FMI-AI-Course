package bg.uni.sofia.fmi.ai.enums;

public enum Attributes {
    AGE("age"),
    MENOPAUSE("menopause"),
    TUMOR_SIZE("tumor-size"),
    INV_NODES("inv-nodes"),
    NODE_CAPS("node-caps"),
    DEG_MALIG("deg-malig"),
    BREAST("breast"),
    BREAST_QUAD("breast-quad"),
    IRRADIAT("irradiat");

    private final String attributeName;

    Attributes(String name) {
        this.attributeName = name;
    }

    public String getAttributeName() {
        return this.attributeName;
    }
}
