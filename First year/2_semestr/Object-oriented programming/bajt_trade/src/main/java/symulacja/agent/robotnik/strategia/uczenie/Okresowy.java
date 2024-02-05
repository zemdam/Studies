package symulacja.agent.robotnik.strategia.uczenie;

import com.squareup.moshi.Json;
import symulacja.Info;
import symulacja.agent.robotnik.Robotnik;

public class Okresowy implements StrategiaNauki {
    @Json(name = "okresowosc_nauki")
    private final int okresowośćNauki;

    public Okresowy(int okresowośćNauki) {
        this.okresowośćNauki = okresowośćNauki;
    }

    @Override
    public boolean czySięUczyć(Info info, Robotnik robotnik) {
        return info.bieżącyDzień() % okresowośćNauki == 0;
    }

    public int okresowośćNauki() {
        return okresowośćNauki;
    }
}
