package symulacja.agent.robotnik.strategia.produkcja;

import symulacja.Info;
import symulacja.agent.robotnik.Robotnik;
import symulacja.zasób.TypProduktu;

public interface StrategiaProdukcji {
    TypProduktu coProdukować(Robotnik robotnik, Info info);
}
