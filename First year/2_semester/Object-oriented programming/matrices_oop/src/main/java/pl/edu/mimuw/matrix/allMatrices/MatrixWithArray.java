package pl.edu.mimuw.matrix.allMatrices;

import java.util.Arrays;

import static java.lang.Math.*;

abstract public class MatrixWithArray extends CommonValueMatrix {
    protected final double[] array;
    protected final int cellsNumber;

    protected MatrixWithArray(int cellsNumber, double... values) {
        double[] array = new double[values.length];
        System.arraycopy(values, 0, array, 0, values.length);
        this.cellsNumber = cellsNumber;
        this.array = array;
    }

    protected double[] newArrayMultiplied(double multiplier) {
        double[] newArray = Arrays.copyOf(array, array.length);

        for (int i = 0; i < array.length; i++) {
            newArray[i] *= multiplier;
        }

        return newArray;
    }

    protected double[] newArrayIncreased(double numberToAdd) {
        double[] newArray = Arrays.copyOf(array, array.length);

        for (int i = 0; i < array.length; i++) {
            newArray[i] += numberToAdd;
        }

        return newArray;
    }

    private double maxAbs() {
        double maxValue = 0;

        for (double v : array) {
            if (abs(v) > maxValue) {
                maxValue = abs(v);
            }
        }

        return maxValue;
    }

    protected double sumAbs() {
        double sum = 0;

        for (double v : array) {
            sum += abs(v);
        }

        return sum;
    }

    @Override
    protected double normOneBetter() {
        return maxAbs() * cellsNumber;
    }

    @Override
    protected double normInfinityBetter() {
        return maxAbs() * cellsNumber;
    }

    @Override
    protected double frobeniusNormBetter() {
        double norm = 0;

        for (double v : array) {
            norm += cellsNumber * pow(v, 2);
        }

        return sqrt(norm);
    }
}
