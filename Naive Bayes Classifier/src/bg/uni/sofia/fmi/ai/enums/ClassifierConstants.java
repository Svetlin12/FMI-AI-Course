package bg.uni.sofia.fmi.ai.enums;

public enum ClassifierConstants {
    FEATURES_COUNT(16), CROSS_FOLD_VALIDATION_NUMBER(10);

    private final int numberConstant;

    ClassifierConstants(int numberConstant) {
        this.numberConstant = numberConstant;
    }

    public int getNumberConstant() {
        return numberConstant;
    }
}
