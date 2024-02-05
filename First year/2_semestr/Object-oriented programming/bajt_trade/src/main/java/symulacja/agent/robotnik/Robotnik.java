package symulacja.agent.robotnik;

import com.squareup.moshi.Json;
import symulacja.Info;
import symulacja.agent.Agent;
import symulacja.agent.robotnik.strategia.kariera.StrategiaKariery;
import symulacja.agent.robotnik.strategia.kupowanie.StrategiaKupna;
import symulacja.agent.robotnik.strategia.produkcja.StrategiaProdukcji;
import symulacja.agent.robotnik.strategia.uczenie.StrategiaNauki;
import symulacja.zasób.Produkt;
import symulacja.zasób.ProduktZPoziomem;
import symulacja.zasób.TypProduktu;
import symulacja.zasób.ZbiórProduktówInterface;

import java.util.EnumMap;
import java.util.LinkedList;
import java.util.Map;

import static java.lang.Math.max;
import static java.lang.Math.min;

public class Robotnik extends Agent {
    private static final int ILOŚĆ_JEDZENIA = 100;
    private static final int ILOŚĆ_UBRAŃ = 100;
    @Json(name = "produktywnosc")
    private final Map<TypProduktu, Integer> produktywność;
    @Json(name = "uczenie")
    private final StrategiaNauki strategiaNauki;
    @Json(name = "zmiana")
    private final StrategiaKariery strategiaKariery;
    @Json(name = "kupowanie")
    private final StrategiaKupna strategiaKupna;
    @Json(name = "produkcja")
    private final StrategiaProdukcji strategiaProdukcji;
    @Json(ignore = true)
    private final Map<TypProduktu, Integer> kariery;
    @Json(ignore = true)
    private int dniBezJedzenia;
    @Json(ignore = true)
    private boolean czyMartwy = false;
    @Json(name = "kariera")
    private TypProduktu aktualnaŚcieżkaKariery;
    @Json(ignore = true)
    private int ostatniaProdukcja;
    private int poziom;

    public Robotnik() {
        produktywność = new EnumMap<>(TypProduktu.class);
        strategiaNauki = null;
        strategiaKariery = null;
        strategiaKupna = null;
        strategiaProdukcji = null;
        kariery = new EnumMap<>(TypProduktu.class);
    }

    public Robotnik(EnumMap<TypProduktu, ZbiórProduktówInterface> zasoby, int id,
                    EnumMap<TypProduktu, Integer> produktywność,
                    StrategiaNauki strategiaNauki, StrategiaKariery strategiaKariery,
                    StrategiaKupna strategiaKupna, StrategiaProdukcji strategiaProdukcji,
                    TypProduktu aktualnaŚcieżkaKariery, int poziom) {
        super(zasoby, id);
        this.produktywność = produktywność;
        this.strategiaNauki = strategiaNauki;
        this.strategiaKariery = strategiaKariery;
        this.strategiaKupna = strategiaKupna;
        this.strategiaProdukcji = strategiaProdukcji;
        this.aktualnaŚcieżkaKariery = aktualnaŚcieżkaKariery;
        kariery = new EnumMap<>(TypProduktu.class);
        kariery.put(aktualnaŚcieżkaKariery, poziom);
        ustawKariery();
    }

    private void ustawKariery() {
        for (TypProduktu typ : TypProduktu.values()) {
            kariery.putIfAbsent(typ, 1);
        }
    }

    private void zgiń() {
        czyMartwy = true;
        liczbaDiamentów = 0;
        zasoby.get(TypProduktu.Diamenty).usuń();
    }

    private int zjedz() {
        int kara;
        switch (dniBezJedzenia) {
            case 1:
                kara = -100;
                break;
            case 2:
                kara = -300;
                break;
            default:
                kara = 0;
                break;
        }

        if (zasoby.get(TypProduktu.Jedzenie).ilość() < ILOŚĆ_JEDZENIA) {
            dniBezJedzenia++;

            if (dniBezJedzenia == 3) {
                zgiń();
            }
        }

        return kara;
    }

    private int ubierzSię(Info info) {
        if (zasoby.get(TypProduktu.Ubrania).ilość() < ILOŚĆ_UBRAŃ) {
            return -info.karaZaBrakUbrań();
        }

        return 0;
    }

    private int użyjNarzędzi() {
        return zasoby.get(TypProduktu.Narzędzia).sumujPoziom();
    }

    private void zużyjZasoby() {
        zasoby.get(TypProduktu.Jedzenie).zmniejszIlość(ILOŚĆ_JEDZENIA);
        zasoby.get(TypProduktu.Narzędzia).usuń();
        zasoby.get(TypProduktu.Ubrania).zmniejszPoziom(ILOŚĆ_UBRAŃ);
    }

    @Override
    public void spędźDzień(Info info) {
        if (czyMartwy) {
            return;
        }

        if (strategiaNauki.czySięUczyć(info, this)) {
            uczSię(info);
        } else {
            pracujISprzedaj(info);
            strategiaKupna.kup(this, info);
        }
    }

    public int obliczIlośćProdukcji(TypProduktu typProduktu, Info info) {
        int ilość = produktywność.get(typProduktu);
        int premia = użyjNarzędzi() + ubierzSię(info) + zjedz() + poznajPremię(typProduktu);

        return max(0, ilość + (ilość * premia) / 100);
    }

    private int ilośćGadżetówNajwyższegoPoziomu() {
        if (zasoby.get(TypProduktu.ProgramyKomputerowe).produktZNajwyższymPoziomem() == null) {
            return 0;
        }

        return zasoby.get(TypProduktu.ProgramyKomputerowe).produktZNajwyższymPoziomem().ilość();
    }

    private int najwyższyPoziomGadżetu() {
        if (zasoby.get(TypProduktu.ProgramyKomputerowe).produktZNajwyższymPoziomem() == null) {
            return 1;
        }

        return zasoby.get(TypProduktu.ProgramyKomputerowe).produktZNajwyższymPoziomem().poziom();
    }

    private void użyjGadżetówNajwyższegoPoziomu(int ile) {
        zasoby.get(TypProduktu.ProgramyKomputerowe).zmniejszIlośćProduktuNajwyższegoPoziomu(ile);
    }

    private LinkedList<Produkt> wytwórzProduktyZPoziomem(TypProduktu typProduktu, int ilość) {
        int ilośćProgramów = ile(TypProduktu.ProgramyKomputerowe);
        int produkcja;
        int używanaLiczbaGadżetów;
        ProduktZPoziomem nowyProdukt;
        LinkedList<Produkt> produkty = new LinkedList<>();

        while (ilość > 0 && ilośćProgramów > 0) {
            używanaLiczbaGadżetów = ilośćGadżetówNajwyższegoPoziomu();
            produkcja = min(ilość, używanaLiczbaGadżetów);
            nowyProdukt = new ProduktZPoziomem(typProduktu, produkcja, najwyższyPoziomGadżetu());
            produkty.addFirst(nowyProdukt);
            użyjGadżetówNajwyższegoPoziomu(produkcja);
            ilość -= produkcja;
            ilośćProgramów -= produkcja;
        }

        if (ilość > 0) {
            int poziom = 1;
            if (typProduktu == TypProduktu.ProgramyKomputerowe && typProduktu == aktualnaŚcieżkaKariery) {
                poziom = this.poziom;
            }
            produkty.addFirst(new ProduktZPoziomem(typProduktu, ilość, poziom));
        }

        return produkty;
    }

    private LinkedList<Produkt> wytwórzProdukty(TypProduktu typProduktu, Info info) {
        ostatniaProdukcja = obliczIlośćProdukcji(typProduktu, info);
        zużyjZasoby();
        if (typProduktu.czyZPoziomem()) {
            return wytwórzProduktyZPoziomem(typProduktu, ostatniaProdukcja);
        }

        LinkedList<Produkt> produkty = new LinkedList<>();
        produkty.addLast(new Produkt(typProduktu, ostatniaProdukcja));

        return produkty;
    }

    private void pracujISprzedaj(Info info) {
        TypProduktu doProdukcji = strategiaProdukcji.coProdukować(this, info);
        LinkedList<Produkt> produkty = wytwórzProdukty(doProdukcji, info);
        for (Produkt produkt : produkty) {
            info.giełda().sprzedaj(produkt, this, info);
        }
    }

    public void zwiększPoziomKariery() {
        poziom++;
    }

    public void zmieńKarieręJeśliTrzeba(TypProduktu typProduktu) {
        if (aktualnaŚcieżkaKariery == typProduktu) {
            zwiększPoziomKariery();
        } else {
            kariery.put(aktualnaŚcieżkaKariery, poziom);
            aktualnaŚcieżkaKariery = typProduktu;
            poziom = kariery.get(typProduktu);
        }
    }

    private void uczSię(Info info) {
        strategiaKariery.WykonajStrategię(this, info);
        dniBezJedzenia = 0;
    }

    public double liczbaDiamentów() {
        return liczbaDiamentów;
    }

    public void dodajZakup(Produkt produkt) {
        dodajProdukt(produkt);
    }

    public int ileBrakuje(TypProduktu typProduktu, int wartość) {
        return wartość - min(wartość, ile(typProduktu));
    }

    public int ostatniaProdukcja() {
        return ostatniaProdukcja;
    }

    private int poznajPremię(TypProduktu typProduktu) {
        if (typProduktu != aktualnaŚcieżkaKariery) {
            return 0;
        }

        switch (poziom) {
            case 1:
                return 50;
            case 2:
                return 150;
            default:
                return 300 + (poziom - 3) * 25;
        }
    }

    public void zakończDzień() {
        zasoby.get(TypProduktu.Diamenty).dodaj((int) liczbaDiamentów);
    }
}