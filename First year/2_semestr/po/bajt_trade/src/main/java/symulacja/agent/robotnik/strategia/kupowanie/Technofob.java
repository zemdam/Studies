package symulacja.agent.robotnik.strategia.kupowanie;

import symulacja.Info;
import symulacja.agent.robotnik.Robotnik;
import symulacja.zasób.TypProduktu;

public class Technofob extends StrategiaKupna {
    @Override
    public void kup(Robotnik robotnik, Info info) {
        info.giełda().kup(TypProduktu.Jedzenie, 100, robotnik, info);
    }
}
