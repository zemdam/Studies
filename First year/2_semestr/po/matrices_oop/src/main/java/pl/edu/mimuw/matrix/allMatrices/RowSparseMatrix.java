package pl.edu.mimuw.matrix.allMatrices;

import pl.edu.mimuw.matrix.IDoubleMatrix;
import pl.edu.mimuw.matrix.MatrixCellValue;
import pl.edu.mimuw.matrix.Shape;

import java.util.Arrays;
import java.util.Comparator;

import static java.lang.Math.*;

public class RowSparseMatrix extends ShapedMatrix {
    private final Rows rows;

    public RowSparseMatrix(Shape shape, MatrixCellValue... values) {
        super(shape);
        rows = new Rows();
        addValues(values);
    }

    private RowSparseMatrix(Shape shape, Rows rows) {
        super(shape);
        this.rows = rows;
    }

    private double findNormOne(MatrixCellValue... values) {
        double tmp = abs(values[0].value);
        double max = tmp;

        for (int i = 1; i < values.length; i++) {
            if (values[i].column != values[i - 1].column) {
                tmp = abs(values[i].value);
            } else {
                tmp += abs(values[i].value);
            }

            if (tmp > max) {
                max = tmp;
            }
        }

        return max;
    }

    private void addValues(MatrixCellValue... values) {
        for (MatrixCellValue value : values) {
            rows.addCell(value);
        }
    }

    @Override
    protected double normOneBetter() {
        MatrixCellValue[] values = rows.toValues(1);
        Arrays.sort(values, new ColumnRowCompare());
        return findNormOne(values);
    }

    @Override
    protected double normInfinityBetter() {
        return rows.maxAbs();
    }

    @Override
    protected double frobeniusNormBetter() {
        return sqrt(rows.pow2ElementSum());
    }

    @Override
    public IDoubleMatrix times(double scalar) {
        MatrixCellValue[] newValues = rows.toValues(scalar);
        return new RowSparseMatrix(matrixShape, newValues);
    }

    @Override
    protected IDoubleMatrix timesAsserted(IDoubleMatrix other) {
        Shape newShape = Shape.matrix(rowsNumber(), other.shape().columns);
        Rows newRows = rows.timesToRows(other);
        return new RowSparseMatrix(newShape, newRows);
    }

    @Override
    protected IDoubleMatrix plusAsserted(IDoubleMatrix other) {
        if (getClass() == other.getClass()) {
            return plusAssertedBetter((RowSparseMatrix) other);
        }
        return super.plusAsserted(other);
    }

    @Override
    protected double getAsserted(int row, int column) {
        return rows.getValue(row, column);
    }

    @Override
    protected String toStringRest() {
        StringBuilder builder = new StringBuilder();
        int currentRow = 0;
        int nonZeroRow;

        for (int i = 0; i < rows.usedSize(); i++) {
            nonZeroRow = rows.rowAtIndex(i);
            builder.append(zeroRows(nonZeroRow - currentRow));
            currentRow = nonZeroRow + 1;
            builder.append(rowToString(i));
        }

        if (currentRow < rowsNumber()) {
            builder.append(zeroRows(rowsNumber() - currentRow));
        }

        return builder.toString();
    }

    private IDoubleMatrix plusAssertedBetter(RowSparseMatrix other) {
        Rows newRows = rows.addRows(other.rows);
        return new RowSparseMatrix(matrixShape, newRows);
    }

    private String rowToString(int rowIndex) {
        StringBuilder builder = new StringBuilder();
        int nonZeroColumn = rows.columnAtIndex(rowIndex, 0);
        int currentColumn = 0;

        if (nonZeroColumn != currentColumn) {
            builder.append(commonValueToString(nonZeroColumn - currentColumn, 0));
            builder.append(" ");
        }

        currentColumn = nonZeroColumn + 1;
        builder.append(rows.getValueAtIndex(rowIndex, 0));

        for (int i = 1; i < rows.columnUsedSize(rowIndex); i++) {
            nonZeroColumn = rows.columnAtIndex(rowIndex, i);

            if (nonZeroColumn != currentColumn) {
                builder.append(" ");
                builder.append(commonValueToString(nonZeroColumn - currentColumn, 0));
            }

            currentColumn = nonZeroColumn + 1;
            builder.append(" ");
            builder.append(rows.getValueAtIndex(rowIndex, i));
        }

        if (currentColumn < columnsNumber()) {
            builder.append(commonValueToString(columnsNumber() - currentColumn, 0));
        }

        builder.append("\n");

        return builder.toString();
    }

    private String zeroRows(int amount) {
        StringBuilder builder = new StringBuilder();

        for (int i = 0; i < amount; i++) {
            builder.append(commonValueToString(columnsNumber(), 0));
            builder.append("\n");
        }

        return builder.toString();
    }
}

class Rows {
    private int totalSize;
    private int usedSize;
    private Row[] rows;

    Rows() {
        totalSize = 1;
        usedSize = 0;
        rows = new Row[1];
    }

    double getValueAtIndex(int rowIndex, int columnIndex) {
        return rows[rowIndex].getValueAtIndex(columnIndex);
    }

    int columnUsedSize(int rowIndex) {
        return rows[rowIndex].usedSize;
    }

    int usedSize() {
        return usedSize;
    }

    int rowAtIndex(int index) {
        return rows[index].rowNumber;
    }

    int columnAtIndex(int rowIndex, int columnIndex) {
        return rows[rowIndex].columnAtIndex(columnIndex);
    }

    double maxAbs() {
        double max = rows[0].sumAbs(1);
        double tmp;

        for (int i = 1; i < usedSize; i++) {
            tmp = rows[i].sumAbs(1);
            if (tmp > max) {
                max = tmp;
            }
        }

        return max;
    }

    Rows addRows(Rows rowsToAdd) {
        Rows newRows = new Rows();
        Row[] rows2 = rowsToAdd.rows;
        int index = 0;
        int index2 = 0;

        while (index < usedSize && index2 < rowsToAdd.usedSize) {
            if (rows[index].rowNumber < rows2[index2].rowNumber) {
                newRows.copyCells(rows[index], 0);
                index++;
            } else if (rows[index].rowNumber > rows2[index2].rowNumber) {
                newRows.copyCells(rows2[index2], 0);
                index2++;
            } else {
                newRows.combineCell(rows[index], rows2[index2]);
                index++;
                index2++;
            }
        }

        newRows.joinRows(this, index);
        newRows.joinRows(rowsToAdd, index2);

        return newRows;
    }

    private void joinRows(Rows rowsToJoin, int index) {
        Row[] rows = rowsToJoin.rows;
        for (; index < rowsToJoin.usedSize; index++) {
            copyCells(rows[index], 0);
        }
    }

    private void combineCell(Row row1, Row row2) {
        int index1 = 0;
        int index2 = 0;
        int row = row1.rowNumber;
        int column;
        double value;
        Row.Column[] columns1 = row1.columns;
        Row.Column[] columns2 = row2.columns;
        MatrixCellValue copiedCell;

        while (index1 < row1.usedSize && index2 < row2.usedSize) {
            column = columns1[index1].columnNumber;
            value = columns1[index1].value;

            if (columns1[index1].columnNumber < columns2[index2].columnNumber) {
                index1++;
            } else if (columns1[index1].columnNumber > columns2[index2].columnNumber) {
                column = columns2[index2].columnNumber;
                value = columns2[index2].value;
                index2++;
            } else {
                value += columns2[index2].value;
                index1++;
                index2++;
            }
            copiedCell = MatrixCellValue.cell(row, column, value);
            addCell(copiedCell);
        }

        copyCells(row1, index1);
        copyCells(row2, index2);
    }

    private void copyCells(Row toCopy, int index) {
        int row = toCopy.rowNumber;
        MatrixCellValue copiedCell;
        int column;
        double value;

        for (; index < toCopy.usedSize; index++) {
            column = toCopy.columns[index].columnNumber;
            value = toCopy.columns[index].value;
            copiedCell = MatrixCellValue.cell(row, column, value);
            addCell(copiedCell);
        }
    }

    double pow2ElementSum() {
        double sum = 0;

        for (int i = 0; i < usedSize; i++) {
            sum += rows[i].sumAbs(2);
        }

        return sum;
    }

    MatrixCellValue[] toValues(double multiplier) {
        MatrixCellValue[] values;
        int size = 0;
        int index = 0;

        for (int i = 0; i < usedSize; i++) {
            size += rows[i].usedSize;
        }

        values = new MatrixCellValue[size];

        for (int i = 0; i < usedSize; i++) {
            for (int j = 0; j < rows[i].usedSize; j++) {
                Row.Column tmp = rows[i].columns[j];
                values[index++] = MatrixCellValue.cell(rows[i].rowNumber, tmp.columnNumber, multiplier * tmp.value);
            }
        }

        return values;
    }

    Rows timesToRows(IDoubleMatrix other) {
        Rows newRows = new Rows();

        for (int i = 0; i < usedSize; i++) {
            for (int j = 0; j < other.shape().columns; j++) {
                newRows.addCell(MatrixCellValue.cell(rows[i].rowNumber, j, rows[i].vectorProduct(j, other)));
            }
        }

        return newRows;
    }

    void addCell(MatrixCellValue cell) {
        createNewRowIfNeeded(cell.row);
        rows[usedSize - 1].addColumn(cell);
    }

    private void createNewRowIfNeeded(int rowNumber) {
        if (usedSize > 0 && rows[usedSize - 1].rowNumber == rowNumber) {
            return;
        }

        if (usedSize >= totalSize) {
            totalSize *= 2;
            rows = Arrays.copyOf(rows, totalSize);
        }

        rows[usedSize++] = new Row(rowNumber);
    }

    double getValue(int row, int column) {
        Row tmpRow = new Row(row);
        int index = Arrays.binarySearch(rows, 0, usedSize, tmpRow);

        if (index < 0) {
            return 0;
        }

        return rows[index].getValue(column);
    }

    private static class Row implements Comparable<Row> {
        private final int rowNumber;
        private int usedSize;
        private int totalSize;
        private Column[] columns;

        private Row(int rowNumber) {
            this.rowNumber = rowNumber;
            usedSize = 0;
            totalSize = 1;
            columns = new Column[1];
        }

        private double getValueAtIndex(int index) {
            return columns[index].value;
        }

        private int columnAtIndex(int index) {
            if (index >= usedSize) {
                return -1;
            }

            return columns[index].columnNumber;
        }

        private double sumAbs(int power) {
            double sum = 0;

            for (int i = 0; i < usedSize; i++) {
                sum += pow(abs(columns[i].value), power);
            }

            return sum;
        }

        private void addColumn(MatrixCellValue cell) {
            if (usedSize >= totalSize) {
                totalSize *= 2;
                columns = Arrays.copyOf(columns, totalSize);
            }

            columns[usedSize++] = new Column(cell.column, cell.value);
        }

        private double getValue(int columnNumber) {
            Column tmpColumn = new Column(columnNumber, 0);
            int index = Arrays.binarySearch(columns, tmpColumn);

            if (index < 0) {
                return 0;
            }

            return columns[index].value;
        }

        private double vectorProduct(int otherColumn, IDoubleMatrix other) {
            double value = 0;

            for (int i = 0; i < usedSize; i++) {
                value += columns[i].value * other.get(columns[i].columnNumber, otherColumn);
            }

            return value;
        }

        @Override
        public int compareTo(Row toCompare) {
            return rowNumber - toCompare.rowNumber;
        }

        private record Column(int columnNumber, double value) implements Comparable<Column> {
            @Override
            public int compareTo(Column toCompare) {
                return columnNumber - toCompare.columnNumber;
            }
        }
    }
}

class ColumnRowCompare implements Comparator<MatrixCellValue> {
    @Override
    public int compare(MatrixCellValue cell1, MatrixCellValue cell2) {
        if (cell1.column < cell2.column) {
            return -1;
        }

        if (cell1.column > cell2.column || cell1.row > cell2.row) {
            return 1;
        }

        if (cell1.row < cell2.row) {
            return -1;
        }

        return 0;
    }
}
