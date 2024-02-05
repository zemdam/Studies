package symulacja.agent.robotnik.strategia.uczenie;

import com.squareup.moshi.Json;
import symulacja.Info;
import symulacja.agent.robotnik.Robotnik;

import java.util.Random;

public class Rozkładowy implements StrategiaNauki {
    @Json(ignore = true)
    private final Random random;

    public Rozkładowy() {
        random = new Random();
    }

    @Override
    public boolean czySięUczyć(Info info, Robotnik robotnik) {
        double prawdopodobieństwoNauki = 1.0 / (info.bieżącyDzień() + 3);
        double losowyDouble = random.nextDouble();

        return losowyDouble < prawdopodobieństwoNauki;
    }
}
