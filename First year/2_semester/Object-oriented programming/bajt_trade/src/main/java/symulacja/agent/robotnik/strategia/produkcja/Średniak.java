package symulacja.agent.robotnik.strategia.produkcja;

import com.squareup.moshi.Json;
import symulacja.Info;
import symulacja.agent.robotnik.Robotnik;
import symulacja.zasób.TypProduktu;

public class Średniak implements StrategiaProdukcji {
    @Json(name = "historia_sredniej_produkcji")
    private final int historiaŚredniejProdukcji;

    public Średniak(int historiaŚredniejProdukcji) {
        this.historiaŚredniejProdukcji = historiaŚredniejProdukcji;
    }

    @Override
    public TypProduktu coProdukować(Robotnik robotnik, Info info) {
        return info.produktZNajwyższąŚrednią(historiaŚredniejProdukcji);
    }
}
