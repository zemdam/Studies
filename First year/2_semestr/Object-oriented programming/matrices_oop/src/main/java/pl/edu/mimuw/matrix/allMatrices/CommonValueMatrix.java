package pl.edu.mimuw.matrix.allMatrices;

abstract public class CommonValueMatrix extends Matrix {
    protected final String commonValueToString(int amount, double commonValue) {
        return switch (amount) {
            case 0 -> "";
            case 1 -> String.valueOf(commonValue);
            case 2 -> commonValue + " " + commonValue;
            default -> commonValue + " ... " + commonValue;
        };
    }
}
