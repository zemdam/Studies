package pl.edu.mimuw.matrix.allMatrices;

import pl.edu.mimuw.matrix.IDoubleMatrix;
import pl.edu.mimuw.matrix.Shape;

import static java.lang.Math.pow;
import static java.lang.Math.sqrt;

public class ConstantMatrix extends ShapedMatrix {
    private final double constantValue;

    public ConstantMatrix(double constantValue, Shape matrixShape) {
        super(matrixShape);
        this.constantValue = constantValue;
    }

    @Override
    protected double normOneBetter() {
        return constantValue * matrixShape.rows;
    }

    @Override
    protected double normInfinityBetter() {
        return constantValue * matrixShape.columns;
    }

    @Override
    protected double frobeniusNormBetter() {
        return sqrt(pow(constantValue, 2) * matrixShape.columns * matrixShape.rows);
    }

    @Override
    public IDoubleMatrix times(double scalar) {
        return new ConstantMatrix(constantValue * scalar, matrixShape);
    }

    @Override
    public IDoubleMatrix plus(double scalar) {
        return new ConstantMatrix(constantValue + scalar, matrixShape);
    }

    @Override
    protected IDoubleMatrix plusAsserted(IDoubleMatrix other) {
        return other.plus(constantValue);
    }

    @Override
    protected double getAsserted(int row, int column) {
        return constantValue;
    }

    @Override
    protected final String toStringRest() {
        StringBuilder builder = new StringBuilder();

        for (int i = 0; i < rowsNumber(); i++) {
            builder.append(commonValueToString(columnsNumber(), constantValue));
            builder.append("\n");
        }

        return builder.toString();
    }
}
