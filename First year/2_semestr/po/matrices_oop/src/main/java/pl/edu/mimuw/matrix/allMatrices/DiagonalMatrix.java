package pl.edu.mimuw.matrix.allMatrices;

import pl.edu.mimuw.matrix.IDoubleMatrix;

import java.util.Arrays;

public class DiagonalMatrix extends MatrixWithArray {

    public DiagonalMatrix(double... diagonalValues) {
        super(1, diagonalValues);
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

        return new DiagonalMatrix(newDiagonal);
    }

    @Override
    public IDoubleMatrix plus(double scalar) {
        double[][] data = new double[rowsNumber()][columnsNumber()];

        for (double[] datum : data) {
            Arrays.fill(datum, scalar);
        }

        for (int i = 0; i < array.length; i++) {
            data[i][i] += array[i];
        }

        return new FullMatrix(data);
    }

    @Override
    protected IDoubleMatrix timesAsserted(IDoubleMatrix other) {
        double[][] data = other.data();
        int size = array.length;

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                data[i][j] *= array[i];
            }
        }

        return new FullMatrix(data);
    }

    @Override
    protected IDoubleMatrix plusAsserted(IDoubleMatrix other) {
        if (getClass() == other.getClass()) {
            return plusAssertedBetter(other);
        }

        double[][] data = other.data();

        for (int i = 0; i < array.length; i++) {
            data[i][i] += array[i];
        }

        return new FullMatrix(data);
    }

    @Override
    protected double getAsserted(int row, int column) {
        if (row == column) {
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
        builder.append(array[0]);
        builder.append(" ");
        builder.append(commonValueToString(array.length - 1, 0));
        builder.append("\n");

        for (int i = 1; i < array.length - 1; i++) {
            builder.append(commonValueToString(i, 0));
            builder.append(" ");
            builder.append(array[i]);
            builder.append(" ");
            builder.append(commonValueToString(array.length - 1 - i, 0));
            builder.append("\n");
        }

        builder.append(commonValueToString(array.length - 1, 0));
        builder.append(" ");
        builder.append(array[array.length - 1]);
        builder.append("\n");

        return builder.toString();
    }

    private IDoubleMatrix plusAssertedBetter(IDoubleMatrix other) {
        double[] diagonal = array.clone();

        for (int i = 0; i < diagonal.length; i++) {
            diagonal[i] += other.get(i, i);
        }

        return new DiagonalMatrix(diagonal);
    }
}
