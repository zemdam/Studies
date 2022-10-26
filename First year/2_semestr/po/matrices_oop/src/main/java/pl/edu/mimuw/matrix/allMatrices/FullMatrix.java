package pl.edu.mimuw.matrix.allMatrices;

public class FullMatrix extends Matrix {
    private final double[][] data;

    public FullMatrix(double[][] data) {
        double[][] copyOfData = new double[data.length][];

        for (int i = 0; i < data.length; i++) {
            copyOfData[i] = data[i].clone();
        }

        this.data = copyOfData;
    }

    @Override
    protected int rowsNumber() {
        return data.length;
    }

    @Override
    protected int columnsNumber() {
        return data[0].length;
    }

    @Override
    protected double getAsserted(int row, int column) {
        return data[row][column];
    }
}
