package symulacja.giełda;

import symulacja.agent.robotnik.Robotnik;

import java.util.List;

public class Zrównoważona extends Giełda {
    private int indeksLewy;
    private int indeksPrawy;
    private boolean czyLewy;

    public Zrównoważona() {
        super();
        czyLewy = false;
    }

    @Override
    protected Robotnik następnyRobotnik(List<Robotnik> robotnicy) {
        if (czyLewy) {
            czyLewy = false;
            return robotnicy.get(indeksLewy++);
        }

        czyLewy = true;
        return robotnicy.get(--indeksPrawy);
    }

    @Override
    protected boolean czyJestRobotnik(List<Robotnik> robotnicy) {
        return indeksLewy < indeksPrawy;
    }

    @Override
    protected void zacznijKolejkęOdNowa(List<Robotnik> robotnicy) {
        indeksLewy = 0;
        indeksPrawy = robotnicy.size();
        czyLewy = false;
    }

    @Override
    public String toString() {
        return "zrownowazona";
    }
}
