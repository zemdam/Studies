package symulacja.giełda;

import org.jetbrains.annotations.NotNull;
import symulacja.agent.spekulant.Spekulant;
import symulacja.zasób.Produkt;
import symulacja.zasób.TypProduktu;

public class Oferta implements Comparable<Oferta> {
    private final Spekulant wystawiający;
    private final double cena;
    private final Produkt produkt;

    public Oferta(Spekulant wystawiający, double cena, Produkt produkt) {
        this.wystawiający = wystawiający;
        this.cena = cena;
        this.produkt = produkt;
    }

    @Override
    public int compareTo(@NotNull Oferta o) {
        if (produkt.compareTo(o.produkt) != 0) {
            return produkt.compareTo(o.produkt);
        }

        if (cena > o.cena) {
            return 1;
        } else if (cena < o.cena) {
            return -1;
        }

        return 0;
    }

    public TypProduktu typProduktu() {
        return produkt.typProduktu();
    }

    public int ilość() {
        return produkt.ilość();
    }

    public void zmniejszIlość(int wartość) {
        produkt.zmniejszIlość(wartość);
    }

    public double cena() {
        return cena;
    }

    public Produkt dajProdukt(int ile) {
        return produkt.zmniejszonyProdukt(ile);
    }

    public Spekulant wystawiający() {
        return wystawiający;
    }
}
