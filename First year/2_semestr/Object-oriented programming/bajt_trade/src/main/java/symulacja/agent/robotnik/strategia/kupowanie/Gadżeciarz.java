package symulacja.agent.robotnik.strategia.kupowanie;

import symulacja.Info;
import symulacja.agent.robotnik.Robotnik;
import symulacja.zasób.TypProduktu;

public class Gadżeciarz extends Zmechanizowany {
    public Gadżeciarz(int liczbaNarzędzi) {
        super(liczbaNarzędzi);
    }

    @Override
    public void kup(Robotnik robotnik, Info info) {
        super.kup(robotnik, info);
        int ile = robotnik.ileBrakuje(TypProduktu.ProgramyKomputerowe, robotnik.ostatniaProdukcja());
        info.giełda().kup(TypProduktu.ProgramyKomputerowe, ile, robotnik, info);
    }
}
