package symulacja.agent.robotnik.strategia.produkcja;

import com.squareup.moshi.Json;
import symulacja.Info;
import symulacja.agent.robotnik.Robotnik;
import symulacja.zasób.TypProduktu;

import java.util.Random;

public class Losowy implements StrategiaProdukcji {
    @Json(ignore = true)
    private final Random random = new Random();

    @Override
    public TypProduktu coProdukować(Robotnik robotnik, Info info) {
        TypProduktu[] typy = TypProduktu.values();
        int indeks = random.nextInt(typy.length);

        return typy[indeks];
    }
}
