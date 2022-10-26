package symulacja.agent.robotnik.strategia.produkcja;

import com.squareup.moshi.Json;
import symulacja.Info;
import symulacja.agent.robotnik.Robotnik;
import symulacja.zasób.TypProduktu;

public class Perspektywiczny implements StrategiaProdukcji {
    @Json(name = "historia_perspektywy")
    private final int historiaPerspektywy;

    public Perspektywiczny(int historiaPerspektywy) {
        this.historiaPerspektywy = historiaPerspektywy;
    }

    @Override
    public TypProduktu coProdukować(Robotnik robotnik, Info info) {
        TypProduktu typProduktuMaks = TypProduktu.Jedzenie;
        double maks = -Double.MAX_VALUE;
        double tmp;
        for (TypProduktu typ : TypProduktu.values()) {
            tmp = info.dajCenęZPoprzedniegoDnia(typ) - info.dajCenęZIluśDniTemu(typ, historiaPerspektywy);
            if (tmp >= maks && typ.czyNaSprzedaż()) {
                maks = tmp;
                typProduktuMaks = typ;
            }
        }

        return typProduktuMaks;
    }
}
