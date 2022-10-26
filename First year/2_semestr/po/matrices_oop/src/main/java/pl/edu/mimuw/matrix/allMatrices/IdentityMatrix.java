package pl.edu.mimuw.matrix.allMatrices;

import pl.edu.mimuw.matrix.IDoubleMatrix;

import java.util.Arrays;

import static java.lang.Math.sqrt;

public class IdentityMatrix extends DiagonalMatrix {
    private final int size;

    public IdentityMatrix(int size) {
        this.size = size;
    }

    @Override
    protected int rowsNumber() {
        return size;
    }

    @Override
    protected int columnsNumber() {
        return size;
    }

    @Override
    public IDoubleMatrix times(double scalar) {
        double[] diagonal = new double[size];
        Arrays.fill(diagonal, scalar);
        return new DiagonalMatrix(diagonal);
    }

    @Override
    public IDoubleMatrix plus(double scalar) {
        double[][] data = new double[rowsNumber()][columnsNumber()];

        for (double[] datum : data) {
            Arrays.fill(datum, scalar);
        }

        for (int i = 0; i < size; i++) {
            data[i][i] += 1;
        }

        return new FullMatrix(data);
    }

    @Override
    protected IDoubleMatrix timesAsserted(IDoubleMatrix other) {
        return other;
    }

    @Override
    protected IDoubleMatrix plusAsserted(IDoubleMatrix other) {
        double[][] data = other.data();

        for (int i = 0; i < size; i++) {
            data[i][i] += 1;
        }

        return new FullMatrix(data);
    }

    @Override
    protected double getAsserted(int row, int column) {
        if (row == column) {
            return 1;
        } else {
            return 0;
        }
    }

    @Override
    protected String toStringRest() {
        if (size == 1) {
            return "1\n";
        }

        StringBuilder builder = new StringBuilder();
        builder.append("1 ");
        builder.append(commonValueToString(size - 1, 0));
        builder.append("\n");

        for (int i = 1; i < size - 1; i++) {
            builder.append(commonValueToString(i, 0));
            builder.append(" 1 ");
            builder.append(commonValueToString(size - 1 - i, 0));
            builder.append("\n");
        }

        builder.append(commonValueToString(size - 1, 0));
        builder.append(" 1\n");

        return builder.toString();
    }

    @Override
    protected double normOneBetter() {
        return 1;
    }

    @Override
    protected double normInfinityBetter() {
        return 1;
    }

    @Override
    protected double frobeniusNormBetter() {
        return sqrt(size);
    }
}
