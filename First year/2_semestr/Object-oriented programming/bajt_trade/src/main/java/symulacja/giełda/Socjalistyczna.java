package symulacja.giełda;

import symulacja.agent.robotnik.Robotnik;

import java.util.List;

public class Socjalistyczna extends Giełda {
    private int indeks;

    public Socjalistyczna() {
        super();
    }

    @Override
    protected Robotnik następnyRobotnik(List<Robotnik> robotnicy) {
        return robotnicy.get(--indeks);
    }

    @Override
    protected boolean czyJestRobotnik(List<Robotnik> robotnicy) {
        return indeks > 0;
    }

    @Override
    protected void zacznijKolejkęOdNowa(List<Robotnik> robotnicy) {
        indeks = robotnicy.size();
    }

    @Override
    public String toString() {
        return "socjalistyczna";
    }
}
