package symulacja.agent.robotnik.strategia.produkcja;

import symulacja.Info;
import symulacja.agent.robotnik.Robotnik;
import symulacja.zasób.TypProduktu;

public class Chciwy implements StrategiaProdukcji {
    @Override
    public TypProduktu coProdukować(Robotnik robotnik, Info info) {
        TypProduktu typProduktuMaks = TypProduktu.Jedzenie;
        double maks = 0;
        double cena;
        int ilość;
        for (TypProduktu typ : TypProduktu.values()) {
            ilość = robotnik.obliczIlośćProdukcji(typ, info);
            cena = info.dajCenęZPoprzedniegoDnia(typ);
            if (ilość * cena >= maks && typ.czyNaSprzedaż()) {
                maks = ilość * cena;
                typProduktuMaks = typ;
            }
        }

        return typProduktuMaks;
    }
}
