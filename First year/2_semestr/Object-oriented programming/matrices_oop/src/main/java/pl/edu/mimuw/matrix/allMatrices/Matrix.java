package pl.edu.mimuw.matrix.allMatrices;

import pl.edu.mimuw.matrix.IDoubleMatrix;
import pl.edu.mimuw.matrix.Shape;

import static java.lang.Math.*;

abstract public class Matrix implements IDoubleMatrix {
    private double normOne = -1;
    private double normInfinity = -1;
    private double frobeniusNorm = -1;

    abstract protected int rowsNumber();

    abstract protected int columnsNumber();

    private boolean isSizeSame(IDoubleMatrix other) {
        return this.rowsNumber() == other.shape().rows && this.columnsNumber() == other.shape().columns;
    }

    private boolean canMultiply(IDoubleMatrix other) {
        return this.columnsNumber() == other.shape().rows;
    }

    private boolean isRowCorrect(int row) {
        return row >= 0 && row < rowsNumber();
    }

    private boolean isColumnCorrect(int column) {
        return column >= 0 && column < columnsNumber();
    }

    protected double normOneBetter() {
        double normOne = 0;
        double tmp;

        for (int j = 0; j < this.columnsNumber(); j++) {
            tmp = 0;
            for (int i = 0; i < this.rowsNumber(); i++) {
                tmp += abs(get(i, j));
            }
            if (tmp > normOne) {
                normOne = tmp;
            }
        }

        return normOne;
    }

    protected double normInfinityBetter() {
        double normInfinity = 0;
        double tmp;

        for (int i = 0; i < this.rowsNumber(); i++) {
            tmp = 0;
            for (int j = 0; j < this.columnsNumber(); j++) {
                tmp += abs(get(i, j));
            }
            if (tmp > normInfinity) {
                normInfinity = tmp;
            }
        }

        return normInfinity;
    }

    protected double frobeniusNormBetter() {
        double norm = 0;

        for (int i = 0; i < this.rowsNumber(); i++) {
            for (int j = 0; j < this.columnsNumber(); j++) {
                norm += pow(get(i, j), 2);
            }
        }

        return sqrt(norm);
    }

    private double vectorProduct(int rowInThis, int columnInOther, IDoubleMatrix other) {
        double vectorProduct = 0;

        for (int i = 0; i < this.columnsNumber(); i++) {
            vectorProduct += this.get(rowInThis, i) * other.get(i, columnInOther);
        }

        return vectorProduct;
    }

    @Override
    public final IDoubleMatrix times(IDoubleMatrix other) {
        assert canMultiply(other);
        return timesAsserted(other);
    }

    @Override
    public IDoubleMatrix times(double scalar) {
        double[][] newData = this.data();

        for (int i = 0; i < this.rowsNumber(); i++) {
            for (int j = 0; j < this.columnsNumber(); j++) {
                newData[i][j] *= scalar;
            }
        }

        return new FullMatrix(newData);
    }

    @Override
    public final IDoubleMatrix plus(IDoubleMatrix other) {
        assert isSizeSame(other);
        return plusAsserted(other);
    }

    @Override
    public IDoubleMatrix plus(double scalar) {
        double[][] newData = this.data();

        for (int i = 0; i < this.rowsNumber(); i++) {
            for (int j = 0; j < this.columnsNumber(); j++) {
                newData[i][j] += scalar;
            }
        }

        return new FullMatrix(newData);
    }

    @Override
    public final IDoubleMatrix minus(IDoubleMatrix other) {
        return plus(other.times(-1));
    }

    @Override
    public final IDoubleMatrix minus(double scalar) {
        return plus(-scalar);
    }

    @Override
    public final double get(int row, int column) {
        assert isRowCorrect(row) && isColumnCorrect(column);
        return getAsserted(row, column);
    }

    @Override
    public final double[][] data() {
        double[][] data = new double[this.rowsNumber()][this.columnsNumber()];

        for (int i = 0; i < this.rowsNumber(); i++) {
            for (int j = 0; j < this.columnsNumber(); j++) {
                data[i][j] = this.get(i, j);
            }
        }

        return data;
    }

    @Override
    public final double normOne() {
        if (normOne == -1) {
            normOne = normOneBetter();
        }

        return normOne;
    }

    @Override
    public final double normInfinity() {
        if (normInfinity == -1) {
            normInfinity = normInfinityBetter();
        }

        return normInfinity;
    }

    @Override
    public final double frobeniusNorm() {
        if (frobeniusNorm == -1) {
            frobeniusNorm = frobeniusNormBetter();
        }

        return frobeniusNorm;
    }

    @Override
    public Shape shape() {
        return Shape.matrix(this.rowsNumber(), this.columnsNumber());
    }

    protected IDoubleMatrix timesAsserted(IDoubleMatrix other) {
        double[][] newData = new double[this.rowsNumber()][other.shape().columns];

        for (int i = 0; i < this.rowsNumber(); i++) {
            for (int j = 0; j < this.rowsNumber(); j++) {
                newData[i][j] = vectorProduct(i, j, other);
            }
        }

        return new FullMatrix(newData);
    }

    protected IDoubleMatrix plusAsserted(IDoubleMatrix other) {
        double[][] newData = data();

        for (int i = 0; i < this.rowsNumber(); i++) {
            for (int j = 0; j < this.columnsNumber(); j++) {
                newData[i][j] += other.get(i, j);
            }
        }

        return new FullMatrix(newData);
    }

    abstract protected double getAsserted(int row, int column);

    @Override
    public final String toString() {
        return "Dimensions: " + rowsNumber() + " x " + columnsNumber() + "\n" + toStringRest();
    }

    protected String toStringRest() {
        StringBuilder builder = new StringBuilder();
        double[][] data = data();

        for (double[] datum : data) {
            builder.append(datum[0]);
            for (int j = 1; j < datum.length; j++) {
                builder.append(" ");
                builder.append(datum[j]);
            }
            builder.append("\n");
        }

        return builder.toString();
    }
}
