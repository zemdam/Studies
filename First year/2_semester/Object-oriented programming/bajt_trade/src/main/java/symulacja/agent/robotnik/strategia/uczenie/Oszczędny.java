package symulacja.agent.robotnik.strategia.uczenie;

import com.squareup.moshi.Json;
import symulacja.Info;
import symulacja.agent.robotnik.Robotnik;

public class Oszczędny implements StrategiaNauki {
    @Json(name = "limit_diamentow")
    private final int limitDiamentów;

    public Oszczędny(int limitDiamentów) {
        this.limitDiamentów = limitDiamentów;
    }

    @Override
    public boolean czySięUczyć(Info info, Robotnik robotnik) {
        return robotnik.liczbaDiamentów() > limitDiamentów;
    }

    public int limitDiamentów() {
        return limitDiamentów;
    }
}
