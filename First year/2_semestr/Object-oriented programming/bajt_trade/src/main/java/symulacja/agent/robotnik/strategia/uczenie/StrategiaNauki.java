package symulacja.agent.robotnik.strategia.uczenie;

import symulacja.Info;
import symulacja.agent.robotnik.Robotnik;

public interface StrategiaNauki {
    boolean czySięUczyć(Info info, Robotnik robotnik);
}
