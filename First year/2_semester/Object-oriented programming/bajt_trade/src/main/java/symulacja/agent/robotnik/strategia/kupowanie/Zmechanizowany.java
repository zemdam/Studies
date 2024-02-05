package symulacja.agent.robotnik.strategia.kupowanie;

import com.squareup.moshi.Json;
import symulacja.Info;
import symulacja.agent.robotnik.Robotnik;
import symulacja.zasób.TypProduktu;

public class Zmechanizowany extends Czyścioszek {
    @Json(name = "liczba_narzedzi")
    private final int liczbaNarzędzi;

    public Zmechanizowany(int liczbaNarzędzi) {
        this.liczbaNarzędzi = liczbaNarzędzi;
    }

    @Override
    public void kup(Robotnik robotnik, Info info) {
        super.kup(robotnik, info);
        info.giełda().kup(TypProduktu.Narzędzia, liczbaNarzędzi, robotnik, info);
    }
}
