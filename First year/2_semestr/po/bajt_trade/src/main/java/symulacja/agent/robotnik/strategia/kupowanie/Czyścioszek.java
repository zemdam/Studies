package symulacja.agent.robotnik.strategia.kupowanie;

import symulacja.Info;
import symulacja.agent.robotnik.Robotnik;
import symulacja.zasób.TypProduktu;

public class Czyścioszek extends Technofob {
    @Override
    public void kup(Robotnik robotnik, Info info) {
        super.kup(robotnik, info);
        int ile = robotnik.ileBrakuje(TypProduktu.Ubrania, 100);
        info.giełda().kup(TypProduktu.Ubrania, ile, robotnik, info);
    }
}
