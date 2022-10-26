package symulacja.giełda;

import symulacja.Info;
import symulacja.agent.robotnik.Robotnik;
import symulacja.agent.spekulant.Spekulant;
import symulacja.zasób.Produkt;
import symulacja.zasób.TypProduktu;

import java.util.*;

import static java.lang.Math.min;

abstract public class Giełda {
    private int numerDnia;
    private Map<TypProduktu, ArrayList<Oferta>> ofertySprzedaży;
    private Map<TypProduktu, ArrayList<Oferta>> ofertyZakupu;

    protected Giełda() {
        TypProduktu[] array = TypProduktu.values();
        ofertySprzedaży = new EnumMap<>(TypProduktu.class);
        ofertyZakupu = new EnumMap<>(TypProduktu.class);

        for (TypProduktu typProduktu : array) {
            ofertyZakupu.put(typProduktu, new ArrayList<>());
            ofertySprzedaży.put(typProduktu, new ArrayList<>());
        }
    }

    private void zresetujOferty() {
        ofertySprzedaży = new EnumMap<>(TypProduktu.class);
        ofertyZakupu = new EnumMap<>(TypProduktu.class);

        for (TypProduktu typProduktu : TypProduktu.values()) {
            ofertyZakupu.put(typProduktu, new ArrayList<>());
            ofertySprzedaży.put(typProduktu, new ArrayList<>());
        }
    }

    public int numerDnia() {
        return numerDnia;
    }

    protected abstract Robotnik następnyRobotnik(List<Robotnik> robotnicy);

    protected abstract boolean czyJestRobotnik(List<Robotnik> robotnicy);

    protected abstract void zacznijKolejkęOdNowa(List<Robotnik> robotnicy);

    public void dodajOfertęSprzedaży(Oferta oferta) {
        ofertySprzedaży.get(oferta.typProduktu()).add(oferta);
    }

    public void dodajOfertęZakupu(Oferta oferta) {
        ofertyZakupu.get(oferta.typProduktu()).add(oferta);
    }

    public void sprzedaj(Produkt produkt, Robotnik robotnik, Info info) {
        int ilość = produkt.ilość();
        int liczbaSprzedaży;
        ArrayList<Oferta> oferty = ofertyZakupu.get(produkt.typProduktu());
        int i = oferty.size() - 1;

        while (ilość > 0 && oferty.size() > 0) {
            liczbaSprzedaży = min(ilość, oferty.get(i).ilość());
            ilość -= liczbaSprzedaży;
            robotnik.zwiększLiczbęDiamentów(liczbaSprzedaży * oferty.get(i).cena());
            oferty.get(i).wystawiający().kup(produkt.zmniejszonyProdukt(liczbaSprzedaży), oferty.get(i).cena());
            oferty.get(i).zmniejszIlość(liczbaSprzedaży);
            info.dodajZakup(produkt.typProduktu(), liczbaSprzedaży, oferty.get(i).cena());

            if (oferty.get(i).ilość() == 0) {
                oferty.remove(i);
            }
        }

        if (ilość > 0) {
            double cena = info.dajCenęZPoprzedniegoDnia(produkt.typProduktu());
            robotnik.zwiększLiczbęDiamentów(ilość * cena);
            info.dodajZakup(produkt.typProduktu(), ilość, cena);
        }
    }

    public void kup(TypProduktu typProduktu, int ilość, Robotnik robotnik, Info info) {
        int liczbaZakupu;
        ArrayList<Oferta> oferty = ofertySprzedaży.get(typProduktu);
        int i = oferty.size() - 1;

        while (ilość > 0 && oferty.size() > 0) {
            liczbaZakupu = min(ilość, oferty.get(i).ilość());
            ilość -= liczbaZakupu;
            robotnik.zmniejszLiczbęDiamentów(liczbaZakupu * oferty.get(i).cena());
            oferty.get(i).wystawiający().zwiększLiczbęDiamentów(liczbaZakupu * oferty.get(i).cena());
            robotnik.dodajZakup(oferty.get(i).dajProdukt(liczbaZakupu));
            info.dodajZakup(typProduktu, liczbaZakupu, oferty.get(i).cena());

            if (oferty.get(i).ilość() == 0) {
                oferty.remove(i);
            }
        }
    }

    public void przeprowadźGiełdę(Info info, List<Robotnik> robotnicy, List<Spekulant> spekulanci) {
        while (numerDnia < info.długość()) {
            zresetujOferty();
            numerDnia++;
            info.zacznijNoweDzienneŚrednie();
            Collections.sort(robotnicy);
            zacznijKolejkęOdNowa(robotnicy);
            Robotnik robotnik;

            for (Spekulant spekulant : spekulanci) {
                spekulant.spędźDzień(info);
            }

            for (TypProduktu typ : TypProduktu.values()) {
                Collections.sort(ofertySprzedaży.get(typ));
                Collections.sort(ofertyZakupu.get(typ), Collections.reverseOrder());
            }

            while (czyJestRobotnik(robotnicy)) {
                robotnik = następnyRobotnik(robotnicy);
                robotnik.spędźDzień(info);
                robotnik.zakończDzień();
            }

            for (Spekulant spekulant : spekulanci) {
                spekulant.zakończDzień();
            }
        }
    }

    @Override
    public abstract String toString();
}
