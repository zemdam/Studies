package symulacja.giełda;

import symulacja.zasób.TypProduktu;

import java.util.EnumMap;
import java.util.Map;

public class ŚredniaCena {
    private final Map<TypProduktu, Średnia> średnieCeny;
    private Średnia najwyższaŚredniaDnia;
    private Średnia największaIlość;

    public ŚredniaCena() {
        średnieCeny = new EnumMap<>(TypProduktu.class);
        TypProduktu[] typy = TypProduktu.values();

        for (TypProduktu typProduktu : typy) {
            średnieCeny.put(typProduktu, new Średnia(typProduktu));
        }
    }

    public ŚredniaCena(ŚredniaCena średniaCena) {
        średnieCeny = new EnumMap<>(TypProduktu.class);

        for (TypProduktu typ : TypProduktu.values()) {
            średnieCeny.put(typ, new Średnia(średniaCena.średnieCeny.get(typ)));
        }
    }

    public void dodajCenę(TypProduktu typProduktu, int ilość, double cena) {
        Średnia średniaDoAktualizacji = średnieCeny.get(typProduktu);
        średniaDoAktualizacji.dodaj(ilość, cena);
    }

    public int ilość(TypProduktu typProduktu) {
        return średnieCeny.get(typProduktu).ilość;
    }

    public double dajCenę(TypProduktu typProduktu) {
        return średnieCeny.get(typProduktu).wartość;
    }

    private void znajdźNajwiększąIlość() {
        if (największaIlość != null) {
            return;
        }

        Średnia maksIlość = new Średnia(TypProduktu.Jedzenie);

        for (TypProduktu typProduktu : TypProduktu.values()) {
            if (typProduktu.czyNaSprzedaż() && maksIlość.ilość <= średnieCeny.get(typProduktu).ilość) {
                maksIlość = średnieCeny.get(typProduktu);
            }
        }

        największaIlość = maksIlość;
    }

    public int maksIlość() {
        znajdźNajwiększąIlość();
        return największaIlość.ilość;
    }

    public TypProduktu typMaksIlości() {
        znajdźNajwiększąIlość();
        return największaIlość.typProduktu;
    }

    private void znajdźNajwyższąŚrednią() {
        if (najwyższaŚredniaDnia != null) {
            return;
        }

        Średnia maksŚrednia = new Średnia(TypProduktu.Jedzenie);

        for (TypProduktu typProduktu : TypProduktu.values()) {
            if (typProduktu.czyNaSprzedaż() && maksŚrednia.wartość <= średnieCeny.get(typProduktu).wartość) {
                maksŚrednia = średnieCeny.get(typProduktu);
            }
        }

        najwyższaŚredniaDnia = maksŚrednia;
    }

    public double wartośćŚredniej() {
        znajdźNajwyższąŚrednią();
        return najwyższaŚredniaDnia.wartość;
    }

    public TypProduktu typNajwyższejŚredniej() {
        znajdźNajwyższąŚrednią();
        return najwyższaŚredniaDnia.typProduktu;
    }

    public boolean porównajWartość(TypProduktu typProduktu, double wartość) {
        znajdźNajwyższąŚrednią();

        if (najwyższaŚredniaDnia.wartość > wartość) {
            return true;
        }

        return największaIlość.ilość == wartość && największaIlość.typProduktu.compareTo(typProduktu) < 0;
    }

    public boolean porównajIlość(TypProduktu typProduktu, int ilość) {
        znajdźNajwiększąIlość();

        if (największaIlość.ilość > ilość) {
            return true;
        }

        return największaIlość.ilość == ilość && największaIlość.typProduktu.compareTo(typProduktu) < 0;
    }

    private static class Średnia {
        private final TypProduktu typProduktu;
        private double wartość;
        private int ilość;

        private Średnia(TypProduktu typProduktu) {
            this.typProduktu = typProduktu;
        }

        private Średnia(Średnia średnia) {
            typProduktu = średnia.typProduktu;
            wartość = średnia.wartość;
            ilość = 0;
        }

        private void dodaj(int ilość, double wartość) {
            this.wartość = this.wartość * this.ilość + wartość * ilość;
            this.ilość += ilość;
            this.wartość /= this.ilość;
        }
    }
}
