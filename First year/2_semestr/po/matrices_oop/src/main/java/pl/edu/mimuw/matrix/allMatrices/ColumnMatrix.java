package pl.edu.mimuw.matrix.allMatrices;

import pl.edu.mimuw.matrix.IDoubleMatrix;

public class ColumnMatrix extends MatrixWithArray {
    public ColumnMatrix(int rowsNumber, double... columnValues) {
        super(rowsNumber, columnValues);
    }

    @Override
    protected int rowsNumber() {
        return array.length;
    }

    @Override
    protected int columnsNumber() {
        return cellsNumber;
    }

    @Override
    public IDoubleMatrix plus(double scalar) {
        double[] newColumnValues = newArrayIncreased(scalar);

        return new ColumnMatrix(cellsNumber, newColumnValues);
    }

    @Override
    protected double getAsserted(int row, int column) {
        return array[row];
    }

    @Override
    protected final String toStringRest() {
        StringBuilder builder = new StringBuilder();

        for (int i = 0; i < rowsNumber(); i++) {
            builder.append(commonValueToString(columnsNumber(), array[i]));
            builder.append("\n");
        }

        return builder.toString();
    }

    @Override
    protected double normOneBetter() {
        return sumAbs();
    }
}
