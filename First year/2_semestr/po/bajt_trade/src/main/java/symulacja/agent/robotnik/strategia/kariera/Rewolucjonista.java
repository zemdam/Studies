package symulacja.agent.robotnik.strategia.kariera;

import symulacja.Info;
import symulacja.agent.robotnik.Robotnik;
import symulacja.zasób.TypProduktu;

import static java.lang.Math.max;

public class Rewolucjonista implements StrategiaKariery {
    @Override
    public void WykonajStrategię(Robotnik robotnik, Info info) {
        if (info.bieżącyDzień() % 7 != 0) {
            robotnik.zwiększPoziomKariery();
            return;
        }

        int n = max(1, robotnik.id() % 17);
        TypProduktu typProduktu = info.produktuZNajwiększąIlością(n);
        robotnik.zmieńKarieręJeśliTrzeba(typProduktu);
    }
}
