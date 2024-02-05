package pl.edu.mimuw.matrix.allMatrices;

import pl.edu.mimuw.matrix.IDoubleMatrix;

import java.util.Arrays;

public class AntiDiagonalMatrix extends MatrixWithArray {
    public AntiDiagonalMatrix(double... antiDiagonalValues) {
        super(1, antiDiagonalValues);
    }

    @Override
    protected int rowsNumber() {
        return array.length;
    }

    @Override
    protected int columnsNumber() {
        return array.length;
    }

    @Override
    public IDoubleMatrix times(double scalar) {
        double[] newDiagonal = newArrayMultiplied(scalar);

        return new AntiDiagonalMatrix(newDiagonal);
    }

    @Override
    public IDoubleMatrix plus(double scalar) {
        double[][] data = new double[rowsNumber()][columnsNumber()];

        for (double[] datum : data) {
            Arrays.fill(datum, scalar);
        }

        for (int i = 0; i < array.length; i++) {
            data[i][columnsNumber() - i - 1] += array[i];
        }

        return new FullMatrix(data);
    }

    @Override
    protected IDoubleMatrix plusAsserted(IDoubleMatrix other) {
        double[][] data = other.data();

        for (int i = 0; i < array.length; i++) {
            data[i][columnsNumber() - i - 1] += array[i];
        }

        return new FullMatrix(data);
    }

    @Override
    protected double getAsserted(int row, int column) {
        if (row == columnsNumber() - column - 1) {
            return array[row];
        } else {
            return 0;
        }
    }

    @Override
    protected String toStringRest() {
        if (array.length == 1) {
            return array[0] + "\n";
        }

        StringBuilder builder = new StringBuilder();
        builder.append(commonValueToString(array.length - 1, 0));
        builder.append(" ");
        builder.append(array[0]);
        builder.append("\n");

        for (int i = 1; i < array.length - 1; i++) {
            builder.append(commonValueToString(array.length - 1 - i, 0));
            builder.append(" ");
            builder.append(array[i]);
            builder.append(" ");
            builder.append(commonValueToString(i, 0));
            builder.append("\n");
        }

        builder.append(array[array.length - 1]);
        builder.append(" ");
        builder.append(commonValueToString(array.length - 1, 0));
        builder.append("\n");

        return builder.toString();
    }
}
