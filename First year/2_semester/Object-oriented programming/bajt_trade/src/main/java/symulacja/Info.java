package symulacja;

import com.squareup.moshi.Json;
import symulacja.giełda.Giełda;
import symulacja.giełda.ŚredniaCena;
import symulacja.zasób.TypProduktu;

import java.util.LinkedList;

import static java.lang.Math.max;
import static java.lang.Math.min;

public class Info {
    @Json(name = "dlugosc")
    private final int długość;
    @Json(name = "gielda")
    private final Giełda giełda;
    @Json(name = "kara_za_brak_ubran")
    private final int karaZaBrakUbrań;
    @Json(name = "ceny")
    private final LinkedList<ŚredniaCena> średnieCeny;

    public Info(int długość, Giełda giełda, int karaZaBrakUbrań, LinkedList<ŚredniaCena> średnieCeny) {
        this.długość = długość;
        this.giełda = giełda;
        this.karaZaBrakUbrań = karaZaBrakUbrań;
        this.średnieCeny = średnieCeny;
    }

    public int długość() {
        return długość;
    }

    public int karaZaBrakUbrań() {
        return karaZaBrakUbrań;
    }

    public int bieżącyDzień() {
        return średnieCeny.size() - 1;
    }

    public double dajCenęZPoprzedniegoDnia(TypProduktu typProduktu) {
        return dajCenęZIluśDniTemu(typProduktu, 0);
    }

    public double dajCenęZIluśDniTemu(TypProduktu typProduktu, int ilośćDni) {
        int indeks = max(0, bieżącyDzień() - 1 - ilośćDni);
        return średnieCeny.get(indeks).dajCenę(typProduktu);
    }

    public void zacznijNoweDzienneŚrednie() {
        średnieCeny.addLast(średnieCeny.getLast());

    }

    public void dodajZakup(TypProduktu typProduktu, int ilość, double cena) {
        średnieCeny.getLast().dodajCenę(typProduktu, ilość, cena);
    }

    private int znajdźPoczątek(int okresDni) {
        return max(0, bieżącyDzień() - okresDni);
    }

    public TypProduktu produktZNajwyższąŚrednią(int okresDni) {
        int początek = znajdźPoczątek(okresDni);
        double maks = 0;
        TypProduktu maksProdukt = TypProduktu.Jedzenie;

        for (; początek < bieżącyDzień(); początek++) {
            if (średnieCeny.get(początek).porównajWartość(maksProdukt, maks)) {
                maksProdukt = średnieCeny.get(początek).typNajwyższejŚredniej();
                maks = średnieCeny.get(początek).wartośćŚredniej();
            }
        }

        return maksProdukt;
    }

    public TypProduktu produktuZNajwiększąIlością(int okresDni) {
        int początek = znajdźPoczątek(okresDni);
        int maks = 0;
        TypProduktu maksProdukt = TypProduktu.Jedzenie;

        for (; początek < bieżącyDzień(); początek++) {
            if (średnieCeny.get(początek).porównajIlość(maksProdukt, maks)) {
                maksProdukt = średnieCeny.get(początek).typMaksIlości();
                maks = średnieCeny.get(początek).maksIlość();
            }
        }

        return maksProdukt;
    }

    public double średniaArytmetyczna(TypProduktu typProduktu, int okresDni) {
        int ilośćDni = min(bieżącyDzień(), okresDni);
        double suma = 0;

        for (int i = 0; i < ilośćDni; i++) {
            suma += dajCenęZIluśDniTemu(typProduktu, i);
        }

        return suma / ilośćDni;
    }

    public int ilośćProduktuDniaObecnego(TypProduktu typProduktu) {
        return średnieCeny.getLast().ilość(typProduktu);
    }

    public int ilośćProduktuDniaPoprzedniego(TypProduktu typProduktu) {
        return średnieCeny.get(bieżącyDzień() - 1).ilość(typProduktu);
    }

    public Giełda giełda() {
        return giełda;
    }
}
