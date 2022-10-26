package symulacja.agent.robotnik.strategia.kariera;

import symulacja.Info;
import symulacja.agent.robotnik.Robotnik;

public class Konserwatysta implements StrategiaKariery {
    @Override
    public void WykonajStrategię(Robotnik robotnik, Info info) {
        robotnik.zwiększPoziomKariery();
    }
}
