package pl.edu.mimuw.matrix.allMatrices;

import pl.edu.mimuw.matrix.IDoubleMatrix;
import pl.edu.mimuw.matrix.Shape;

public class ZeroMatrix extends ConstantMatrix {
    public ZeroMatrix(Shape matrixShape) {
        super(0, matrixShape);
    }

    @Override
    public IDoubleMatrix times(double scalar) {
        return this;
    }

    @Override
    protected IDoubleMatrix plusAsserted(IDoubleMatrix other) {
        return other;
    }

    @Override
    protected double normOneBetter() {
        return 0;
    }

    @Override
    protected double normInfinityBetter() {
        return 0;
    }

    @Override
    protected double frobeniusNormBetter() {
        return 0;
    }

    @Override
    protected IDoubleMatrix timesAsserted(IDoubleMatrix other) {
        Shape newMatrixShape = Shape.matrix(this.rowsNumber(), other.shape().columns);
        return new ZeroMatrix(newMatrixShape);
    }
}
