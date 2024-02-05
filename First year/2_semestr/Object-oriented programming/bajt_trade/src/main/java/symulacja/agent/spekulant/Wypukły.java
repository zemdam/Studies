package symulacja.agent.spekulant;

import symulacja.Info;
import symulacja.zasób.TypProduktu;
import symulacja.zasób.ZbiórProduktówInterface;

import java.util.EnumMap;

public class Wypukły extends Spekulant {
    public Wypukły(EnumMap<TypProduktu, ZbiórProduktówInterface> zasoby, int id) {
        super(zasoby, id);
    }

    public Wypukły() {
        super();
    }

    private boolean czyWypukła(TypProduktu typProduktu, Info info) {
        double cena0Dzień = info.dajCenęZIluśDniTemu(typProduktu, 0);
        double cena1Dzień = info.dajCenęZIluśDniTemu(typProduktu, 1);
        double cena2Dzień = info.dajCenęZIluśDniTemu(typProduktu, 2);

        return cena0Dzień > cena1Dzień && cena2Dzień > cena1Dzień;
    }

    private boolean czyWklęsła(TypProduktu typProduktu, Info info) {
        double cena0Dzień = info.dajCenęZIluśDniTemu(typProduktu, 0);
        double cena1Dzień = info.dajCenęZIluśDniTemu(typProduktu, 1);
        double cena2Dzień = info.dajCenęZIluśDniTemu(typProduktu, 2);

        return cena0Dzień < cena1Dzień && cena2Dzień < cena1Dzień;
    }

    @Override
    protected void wystawOferty(Info info) {
        if (info.bieżącyDzień() < 3) {
            return;
        }

        super.wystawOferty(info);
    }

    @Override
    protected double obliczCenę(TypProduktu typProduktu, Info info) {
        return info.dajCenęZPoprzedniegoDnia(typProduktu);
    }

    @Override
    protected void dodajNaGiełdę(TypProduktu typProduktu, Info info, double cena) {
        if (czyWypukła(typProduktu, info)) {
            ofertaKupna(typProduktu, info.giełda(), cena, MNOŻNIK_KUPNA);
        } else if (czyWklęsła(typProduktu, info)) {
            ofertaSprzedaży(typProduktu, info.giełda(), cena, MNOŻNIK_SPRZEDAŻY);
        }
    }
}
