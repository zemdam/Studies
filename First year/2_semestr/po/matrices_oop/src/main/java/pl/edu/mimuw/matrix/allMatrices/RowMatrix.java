package pl.edu.mimuw.matrix.allMatrices;

import pl.edu.mimuw.matrix.IDoubleMatrix;

public class RowMatrix extends MatrixWithArray {

    public RowMatrix(int columnsNumber, double... rowValues) {
        super(columnsNumber, rowValues);
    }

    @Override
    protected int rowsNumber() {
        return cellsNumber;
    }

    @Override
    protected int columnsNumber() {
        return array.length;
    }

    @Override
    public IDoubleMatrix plus(double scalar) {
        double[] newRowValues = newArrayIncreased(scalar);

        return new RowMatrix(cellsNumber, newRowValues);
    }

    @Override
    protected double getAsserted(int row, int column) {
        return array[column];
    }

    @Override
    protected double normInfinityBetter() {
        return sumAbs();
    }
}
