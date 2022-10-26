package symulacja.giełda;

import symulacja.agent.robotnik.Robotnik;

import java.util.List;

public class Kapitalistyczna extends Giełda {
    private int indeks;

    public Kapitalistyczna() {
        super();
        indeks = 0;
    }

    @Override
    protected Robotnik następnyRobotnik(List<Robotnik> robotnicy) {
        return robotnicy.get(indeks++);
    }

    @Override
    protected boolean czyJestRobotnik(List<Robotnik> robotnicy) {
        return indeks < robotnicy.size();
    }

    @Override
    protected void zacznijKolejkęOdNowa(List<Robotnik> robotnicy) {
        indeks = 0;
    }

    @Override
    public String toString() {
        return "kapitalistyczna";
    }
}
