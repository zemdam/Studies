package symulacja.agent.spekulant;

import symulacja.Info;
import symulacja.zasób.TypProduktu;
import symulacja.zasób.ZbiórProduktówInterface;

import java.util.EnumMap;

import static java.lang.Math.max;

public class Regulujący extends Spekulant {
    public Regulujący(EnumMap<TypProduktu, ZbiórProduktówInterface> zasoby, int id) {
        super(zasoby, id);
    }

    public Regulujący() {
        super();
    }

    @Override
    protected double obliczCenę(TypProduktu typProduktu, Info info) {
        double mnożnik = info.ilośćProduktuDniaObecnego(typProduktu);
        mnożnik /= max(1, info.ilośćProduktuDniaPoprzedniego(typProduktu));

        return info.dajCenęZPoprzedniegoDnia(typProduktu) * mnożnik;
    }

    @Override
    protected void dodajNaGiełdę(TypProduktu typProduktu, Info info, double cena) {
        ofertaKupna(typProduktu, info.giełda(), cena, MNOŻNIK_KUPNA);
        ofertaSprzedaży(typProduktu, info.giełda(), cena, MNOŻNIK_SPRZEDAŻY);
    }

    @Override
    protected void wystawOferty(Info info) {
        if (info.bieżącyDzień() < 2) {
            return;
        }

        super.wystawOferty(info);
    }
}
