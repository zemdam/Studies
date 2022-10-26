package symulacja.zasób;

import org.jetbrains.annotations.NotNull;

public class Produkt implements Comparable<Produkt> {
    private final TypProduktu typProduktu;
    private int ilość;

    public Produkt(TypProduktu typProduktu, int ilość) {
        this.typProduktu = typProduktu;
        this.ilość = ilość;
    }

    public Produkt(Produkt produkt) {
        typProduktu = produkt.typProduktu;
        ilość = produkt.ilość;
    }

    public TypProduktu typProduktu() {
        return typProduktu;
    }

    public int ilość() {
        return ilość;
    }

    public int poziom() {
        return 0;
    }

    public void zmniejszIlość(int wartość) {
        ilość -= wartość;
    }

    public void zwiększIlość(int wartość) {
        ilość += wartość;
    }

    public void wyzerujIlość() {
        ilość = 0;
    }

    public void zmniejszPoziom() {
    }

    @Override
    public int compareTo(@NotNull Produkt o) {
        if (poziom() < o.poziom()) {
            return 1;
        } else if (poziom() > o.poziom()) {
            return -1;
        }

        return 0;
    }

    public Produkt zmniejszonyProdukt(int wartość) {
        zmniejszIlość(wartość);

        return new Produkt(typProduktu, wartość);
    }
}
