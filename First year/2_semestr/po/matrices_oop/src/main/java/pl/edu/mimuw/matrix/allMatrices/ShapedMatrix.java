package pl.edu.mimuw.matrix.allMatrices;

import pl.edu.mimuw.matrix.Shape;

abstract public class ShapedMatrix extends CommonValueMatrix {
    protected final Shape matrixShape;

    protected ShapedMatrix(Shape matrixShape) {
        this.matrixShape = Shape.matrix(matrixShape.rows, matrixShape.columns);
    }

    @Override
    protected final int rowsNumber() {
        return matrixShape.rows;
    }

    @Override
    protected final int columnsNumber() {
        return matrixShape.columns;
    }

    @Override
    public final Shape shape() {
        return matrixShape;
    }
}
