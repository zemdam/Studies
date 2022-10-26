package symulacja;

import symulacja.agent.robotnik.Robotnik;
import symulacja.agent.spekulant.Spekulant;

import java.util.ArrayList;
import java.util.List;

public class Symulacja {
    private final Info info;
    private final List<Robotnik> robotnicy;
    private final List<Spekulant> spekulanci;

    public Symulacja(Info info, ArrayList<Robotnik> robotnicy, ArrayList<Spekulant> spekulanci) {
        this.info = info;
        this.robotnicy = robotnicy;
        this.spekulanci = spekulanci;
    }

    public void przeprowadźSymulację() {
        info.giełda().przeprowadźGiełdę(info, robotnicy, spekulanci);
    }
}
