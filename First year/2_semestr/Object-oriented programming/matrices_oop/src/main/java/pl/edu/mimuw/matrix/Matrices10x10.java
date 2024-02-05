package pl.edu.mimuw.matrix;

import static pl.edu.mimuw.matrix.MatrixCellValue.cell;

public class Matrices10x10 {
    public static final IDoubleMatrix ANTI_DIAGONAL_10x10 = DoubleMatrixFactory.antiDiagonal(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

    public static final IDoubleMatrix DIAGONAL_10x10 = DoubleMatrixFactory.diagonal(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

    public static final IDoubleMatrix COLUMN_10x10 = DoubleMatrixFactory.column(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

    public static final IDoubleMatrix ROW_10x10 = DoubleMatrixFactory.row(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

    public static final IDoubleMatrix CONSTANT_10x10 = DoubleMatrixFactory.constant(7, Shape.matrix(10, 10));

    public static final IDoubleMatrix FULL_MATRIX_10x10 = DoubleMatrixFactory.full(DATA_10x10());

    public static final IDoubleMatrix IDENTITY_10x10 = DoubleMatrixFactory.identity(10);

    public static final IDoubleMatrix SPARSE_10x10 = DoubleMatrixFactory.sparse(Shape.matrix(10, 10), cell(5, 2, 3), cell(9, 9, 1));

    private static double[][] DATA_10x10() {
        int value = 1;
        double[][] data = new double[10][10];

        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                data[i][j] = value++;
            }
        }

        return data;
    }
}
