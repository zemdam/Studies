package pl.edu.mimuw.matrix;

public final class MatrixCellValue {

    public final int row;
    public final int column;
    public final double value;

    public MatrixCellValue(int row, int column, double value) {
        this.column = column;
        this.row = row;
        this.value = value;
    }

    public static MatrixCellValue cell(int row, int column, double value) {
        return new MatrixCellValue(row, column, value);
    }

    @Override
    public String toString() {
        return "{" + value + " @[" + row + ", " + column + "]}";
    }

    public boolean isSame(MatrixCellValue cellValue) {
        return row == cellValue.row && column == cellValue.column;
    }

    public boolean isInShape(Shape shape) {
        return row < 0 || row >= shape.rows || column < 0 || column >= shape.columns;
    }
}