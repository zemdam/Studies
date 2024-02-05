package symulacja.agent.spekulant;

import com.squareup.moshi.Json;
import symulacja.Info;
import symulacja.zasób.TypProduktu;

public class Średni extends Spekulant {
    private final static double MNOŻNIK_SPECJALNY = 0.95;
    @Json(name = "historia_spekulanta_sredniego")
    private final int historiaSpekulantaŚredniego;

    public Średni(int historiaSpekulantaŚredniego) {
        this.historiaSpekulantaŚredniego = historiaSpekulantaŚredniego;
    }

    public Średni() {
        super();
        historiaSpekulantaŚredniego = 1;
    }

    @Override
    public double obliczCenę(TypProduktu typProduktu, Info info) {
        return info.średniaArytmetyczna(typProduktu, historiaSpekulantaŚredniego);
    }

    @Override
    public void dodajNaGiełdę(TypProduktu typProduktu, Info info, double cena) {
        if (ile(typProduktu) == 0) {
            ofertaKupna(typProduktu, info.giełda(), cena, MNOŻNIK_SPECJALNY);
            return;
        }

        ofertaKupna(typProduktu, info.giełda(), cena, MNOŻNIK_KUPNA);
        ofertaSprzedaży(typProduktu, info.giełda(), cena, MNOŻNIK_SPRZEDAŻY);
    }
}
