package symulacja.agent.robotnik.strategia.uczenie;

import symulacja.Info;
import symulacja.agent.robotnik.Robotnik;
import symulacja.zasób.TypProduktu;

public class Student implements StrategiaNauki {
    private final int okres;
    private final int zapas;

    public Student() {
        okres = 0;
        zapas = 0;
    }

    public Student(int okres, int zapas) {
        this.okres = okres;
        this.zapas = zapas;
    }

    @Override
    public boolean czySięUczyć(Info info, Robotnik robotnik) {
        double cena = info.średniaArytmetyczna(TypProduktu.Jedzenie, okres);

        return robotnik.liczbaDiamentów() >= 100 * zapas * cena;
    }

    public int okres() {
        return okres;
    }

    public int zapas() {
        return zapas;
    }
}
