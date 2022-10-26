package pl.edu.mimuw.matrix;

import pl.edu.mimuw.matrix.allMatrices.*;

import java.util.Arrays;
import java.util.Comparator;

public class DoubleMatrixFactory {

    private DoubleMatrixFactory() {
    }

    private static boolean checkLengths(double[][] data) {
        for (int i = 0; i < data.length - 1; i++) {
            if (data[i].length != data[i + 1].length) {
                return false;
            }
        }

        return true;
    }

    private static boolean noRepeatsAndInShape(Shape shape, MatrixCellValue... values) {
        MatrixCellValue tmp = values[0];
        if (tmp.isInShape(shape)) {
            return false;
        }

        for (int i = 1; i < values.length; i++) {
            if (values[i].isSame(tmp) || values[i].isInShape(shape)) {
                return false;
            }
            tmp = values[i];
        }

        return true;
    }

    public static IDoubleMatrix sparse(Shape shape, MatrixCellValue... values) {
        assert values != null;
        if (values.length == 0) {
            return zero(shape);
        }
        Arrays.sort(values, new RowColumnCompare());
        assert noRepeatsAndInShape(shape, values);
        return new RowSparseMatrix(shape, values);
    }

    public static IDoubleMatrix full(double[][] values) {
        assert values != null;
        assert values.length > 0;
        assert checkLengths(values);
        return new FullMatrix(values);
    }

    public static IDoubleMatrix identity(int size) {
        assert size > 0;
        return new IdentityMatrix(size);
    }

    public static IDoubleMatrix diagonal(double... diagonalValues) {
        assert diagonalValues != null;
        assert diagonalValues.length > 0;
        return new DiagonalMatrix(diagonalValues);
    }

    public static IDoubleMatrix antiDiagonal(double... antiDiagonalValues) {
        assert antiDiagonalValues != null;
        assert antiDiagonalValues.length > 0;
        return new AntiDiagonalMatrix(antiDiagonalValues);
    }

    public static IDoubleMatrix vector(double... values) {
        assert values != null;
        assert values.length > 0;
        return new ColumnMatrix(1, values);
    }

    public static IDoubleMatrix zero(Shape shape) {
        return new ZeroMatrix(shape);
    }

    public static IDoubleMatrix column(int rowsNumber, double... columnValues) {
        assert rowsNumber > 0;
        assert columnValues != null;
        assert columnValues.length > 0;
        return new ColumnMatrix(rowsNumber, columnValues);
    }

    public static IDoubleMatrix row(int columnsNumber, double... columnValues) {
        assert columnsNumber > 0;
        assert columnValues != null;
        assert columnValues.length > 0;
        return new RowMatrix(columnsNumber, columnValues);
    }

    public static IDoubleMatrix constant(int constantValue, Shape shape) {
        return new ConstantMatrix(constantValue, shape);
    }

    private static class RowColumnCompare implements Comparator<MatrixCellValue> {
        @Override
        public int compare(MatrixCellValue cell1, MatrixCellValue cell2) {
            if (cell1.row < cell2.row) {
                return -1;
            }

            if (cell1.row > cell2.row || cell1.column > cell2.column) {
                return 1;
            }

            if (cell1.column < cell2.column) {
                return -1;
            }

            return 0;
        }
    }
}
