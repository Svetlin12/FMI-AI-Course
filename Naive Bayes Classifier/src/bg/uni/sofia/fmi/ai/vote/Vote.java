package bg.uni.sofia.fmi.ai.vote;

public record Vote(String name, Boolean handicappedInfants, Boolean waterProjectCostSharing,
                   Boolean adoptionOfTheBudgetResolution, Boolean physicianFeeFreeze,
                   Boolean elSalvadorAir, Boolean religiousGroupsInSchools,
                   Boolean antiSatelliteTestBan, Boolean aidToNicaraguanContras,
                   Boolean mxMissile, Boolean immigration, Boolean synfuelsCorporationCutback,
                   Boolean educationSpending, Boolean superfundRightToSue, Boolean crime,
                   Boolean dutyFreeExports, Boolean exportAdministrationActSouthAfrica) {

    private static final int NAME_INDEX = 0;
    private static final int FIRST_VOTE_COMPONENT_INDEX = 1;
    private static final int SECOND_VOTE_COMPONENT_INDEX = 2;
    private static final int THIRD_VOTE_COMPONENT_INDEX = 3;
    private static final int FOURTH_VOTE_COMPONENT_INDEX = 4;
    private static final int FIFTH_VOTE_COMPONENT_INDEX = 5;
    private static final int SIXTH_VOTE_COMPONENT_INDEX = 6;
    private static final int SEVENTH_VOTE_COMPONENT_INDEX = 7;
    private static final int EIGHTH_VOTE_COMPONENT_INDEX = 8;
    private static final int NINTH_VOTE_COMPONENT_INDEX = 9;
    private static final int TENTH_VOTE_COMPONENT_INDEX = 10;
    private static final int ELEVENTH_VOTE_COMPONENT_INDEX = 11;
    private static final int TWELFTH_VOTE_COMPONENT_INDEX = 12;
    private static final int THIRTEENTH_VOTE_COMPONENT_INDEX = 13;
    private static final int FOURTEENTH_VOTE_COMPONENT_INDEX = 14;
    private static final int FIFTEENTH_VOTE_COMPONENT_INDEX = 15;
    private static final int SIXTEENTH_VOTE_COMPONENT_INDEX = 16;

    public static Vote of(String line) {
        String[] fields = line.split(",");

        String name = fields[NAME_INDEX];
        Boolean handicappedInfants = determineBooleanValue(fields[FIRST_VOTE_COMPONENT_INDEX]);
        Boolean waterProjectCostSharing = determineBooleanValue(fields[SECOND_VOTE_COMPONENT_INDEX]);
        Boolean adoptionOfTheBudgetResolution = determineBooleanValue(fields[THIRD_VOTE_COMPONENT_INDEX]);
        Boolean physicianFeeFreeze = determineBooleanValue(fields[FOURTH_VOTE_COMPONENT_INDEX]);
        Boolean elSalvadorAir = determineBooleanValue(fields[FIFTH_VOTE_COMPONENT_INDEX]);
        Boolean religiousGroupsInSchools = determineBooleanValue(fields[SIXTH_VOTE_COMPONENT_INDEX]);
        Boolean antiSatelliteTestBan = determineBooleanValue(fields[SEVENTH_VOTE_COMPONENT_INDEX]);
        Boolean aidToNicaraguanContras = determineBooleanValue(fields[EIGHTH_VOTE_COMPONENT_INDEX]);
        Boolean mxMissile = determineBooleanValue(fields[NINTH_VOTE_COMPONENT_INDEX]);
        Boolean immigration = determineBooleanValue(fields[TENTH_VOTE_COMPONENT_INDEX]);
        Boolean synfuelsCorporationCutback = determineBooleanValue(fields[ELEVENTH_VOTE_COMPONENT_INDEX]);
        Boolean educationSpending = determineBooleanValue(fields[TWELFTH_VOTE_COMPONENT_INDEX]);
        Boolean superfundRightToSue = determineBooleanValue(fields[THIRTEENTH_VOTE_COMPONENT_INDEX]);
        Boolean crime = determineBooleanValue(fields[FOURTEENTH_VOTE_COMPONENT_INDEX]);
        Boolean dutyFreeExports = determineBooleanValue(fields[FIFTEENTH_VOTE_COMPONENT_INDEX]);
        Boolean exportAdministrationActSouthAfrica = determineBooleanValue(fields[SIXTEENTH_VOTE_COMPONENT_INDEX]);

        return new Vote(name, handicappedInfants, waterProjectCostSharing, adoptionOfTheBudgetResolution, physicianFeeFreeze,
                        elSalvadorAir, religiousGroupsInSchools, antiSatelliteTestBan, aidToNicaraguanContras, mxMissile,
                        immigration, synfuelsCorporationCutback, educationSpending, superfundRightToSue, crime,
                        dutyFreeExports, exportAdministrationActSouthAfrica);
    }

    private static Boolean determineBooleanValue(String field) {
        switch(field) {
            case "y" -> {
                return true;
            }
            case "n" -> {
                return false;
            }
            default -> {
                return null;
            }
        }
    }

    public Boolean getVoteAt(int index) {
        Boolean field = false;

        switch(index) {
            case 0 -> field = handicappedInfants();
            case 1 -> field = waterProjectCostSharing();
            case 2 -> field = adoptionOfTheBudgetResolution();
            case 3 -> field = physicianFeeFreeze();
            case 4 -> field = elSalvadorAir();
            case 5 -> field = religiousGroupsInSchools();
            case 6 -> field = antiSatelliteTestBan();
            case 7 -> field = aidToNicaraguanContras();
            case 8 -> field = mxMissile();
            case 9 -> field = immigration();
            case 10 -> field = synfuelsCorporationCutback();
            case 11 -> field = educationSpending();
            case 12 -> field = superfundRightToSue();
            case 13 -> field = crime();
            case 14 -> field = dutyFreeExports();
            case 15 -> field = exportAdministrationActSouthAfrica();
        }

        return field;
    }
}
