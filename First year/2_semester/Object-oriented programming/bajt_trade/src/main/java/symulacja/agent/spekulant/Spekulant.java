package symulacja.agent.spekulant;

import symulacja.Info;
import symulacja.agent.Agent;
import symulacja.giełda.Giełda;
import symulacja.giełda.Oferta;
import symulacja.zasób.Produkt;
import symulacja.zasób.ProduktZPoziomem;
import symulacja.zasób.TypProduktu;
import symulacja.zasób.ZbiórProduktówInterface;

import java.util.EnumMap;

abstract public class Spekulant extends Agent {
    protected static final int DOMYŚLNA_ILOŚĆ_ZAKUPU = 100;
    protected static final double MNOŻNIK_KUPNA = 0.9;
    protected static final double MNOŻNIK_SPRZEDAŻY = 1.1;

    protected Spekulant(EnumMap<TypProduktu, ZbiórProduktówInterface> zasoby, int id) {
        super(zasoby, id);
    }

    protected Spekulant() {
        super();
    }

    public void kup(Produkt produkt, double cena) {
        liczbaDiamentów -= produkt.ilość() * cena;
        dodajProdukt(produkt);
    }

    public void zakończDzień() {
        wyczyść();
        zasoby.get(TypProduktu.Diamenty).dodaj((int) liczbaDiamentów);
    }

    private Produkt nowyProdukt(TypProduktu typProduktu, int ilość) {
        Produkt nowyProdukt;

        if (typProduktu.czyZPoziomem()) {
            nowyProdukt = new ProduktZPoziomem(typProduktu, ilość, 1);
        } else {
            nowyProdukt = new Produkt(typProduktu, ilość);
        }

        return nowyProdukt;
    }

    protected void ofertaKupna(TypProduktu typProduktu, Giełda giełda, double cena, double mnożnik) {
        Produkt produktDoKupienia = nowyProdukt(typProduktu, DOMYŚLNA_ILOŚĆ_ZAKUPU);
        giełda.dodajOfertęZakupu(new Oferta(this, cena * mnożnik, produktDoKupienia));
    }

    protected void ofertaSprzedaży(TypProduktu typProduktu, Giełda giełda, double cena, double mnożnik) {
        for (Produkt produkt : dajProdukty(typProduktu)) {
            giełda.dodajOfertęSprzedaży(new Oferta(this, cena * mnożnik, produkt));
        }
    }

    protected abstract double obliczCenę(TypProduktu typProduktu, Info info);

    protected abstract void dodajNaGiełdę(TypProduktu typProduktu, Info info, double cena);

    protected void wystawOferty(Info info) {
        double cena;
        for (TypProduktu typ : TypProduktu.values()) {
            if (typ.czyNaSprzedaż()) {
                cena = obliczCenę(typ, info);
                dodajNaGiełdę(typ, info, cena);
            }
        }
    }

    @Override
    public void spędźDzień(Info info) {
        wystawOferty(info);
    }
}
