package symulacja.adapter;

import com.squareup.moshi.FromJson;
import com.squareup.moshi.JsonDataException;
import com.squareup.moshi.ToJson;
import symulacja.agent.robotnik.strategia.kariera.Konserwatysta;
import symulacja.agent.robotnik.strategia.kariera.Rewolucjonista;
import symulacja.agent.robotnik.strategia.kariera.StrategiaKariery;
import symulacja.giełda.*;
import symulacja.zasób.TypProduktu;
import symulacja.zasób.ZbiórProduktów;
import symulacja.zasób.ZbiórProduktówInterface;
import symulacja.zasób.ZbiórProduktówZPoziomem;

import java.util.EnumMap;
import java.util.LinkedList;
import java.util.Map;

public class SymulacjaAdapter {
    @FromJson
    Giełda fromJsonGiełda(String giełda) {
        switch (giełda) {
            case "socjalistyczna":
                return new Socjalistyczna();
            case "kapitalistyczna":
                return new Kapitalistyczna();
            case "zrownowazona":
                return new Zrównoważona();
            default:
                throw new JsonDataException();
        }
    }

    @ToJson
    String toJsonGiełda(Giełda giełda) {
        return giełda.toString();
    }

    @FromJson
    LinkedList<ŚredniaCena> fromJsonLista(CenyJson json) {
        LinkedList<ŚredniaCena> ceny = new LinkedList<>();
        ceny.add(new ŚredniaCena());
        ceny.getLast().dodajCenę(TypProduktu.ProgramyKomputerowe, 1, json.programy);
        ceny.getLast().dodajCenę(TypProduktu.Jedzenie, 1, json.jedzenie);
        ceny.getLast().dodajCenę(TypProduktu.Ubrania, 1, json.ubrania);
        ceny.getLast().dodajCenę(TypProduktu.Narzędzia, 1, json.narzedzia);

        return ceny;
    }

    @ToJson
    CenyJson toJsonLista(LinkedList<ŚredniaCena> list) {
        CenyJson json = new CenyJson();
        json.programy = list.getLast().dajCenę(TypProduktu.ProgramyKomputerowe);
        json.jedzenie = list.getLast().dajCenę(TypProduktu.Jedzenie);
        json.ubrania = list.getLast().dajCenę(TypProduktu.Ubrania);
        json.narzedzia = list.getLast().dajCenę(TypProduktu.Narzędzia);

        return json;
    }

    @FromJson
    StrategiaKariery fromJsonKariera(String json) {
        switch (json) {
            case "konserwatysta":
                return new Konserwatysta();
            case "rewolucjonista":
                return new Rewolucjonista();
            default:
                throw new JsonDataException();
        }
    }

    @ToJson
    String toJsonKariera(StrategiaKariery strategiaKariery) {
        if (Konserwatysta.class.equals(strategiaKariery.getClass())) {
            return "konserwatysta";
        } else if (Rewolucjonista.class.equals(strategiaKariery.getClass())) {
            return "rewolucjonista";
        }
        throw new JsonDataException();
    }

    @FromJson
    Map<TypProduktu, ZbiórProduktówInterface> fromJsonZasoby(ZasobyJson json) {
        EnumMap<TypProduktu, ZbiórProduktówInterface> zasoby = new EnumMap<>(TypProduktu.class);
        for (TypProduktu typ : TypProduktu.values()) {
            if (typ.czyZPoziomem()) {
                zasoby.put(typ, new ZbiórProduktówZPoziomem(typ));
            } else {
                zasoby.put(typ, new ZbiórProduktów(typ));
            }
        }
        zasoby.get(TypProduktu.ProgramyKomputerowe).dodaj(json.programy);
        zasoby.get(TypProduktu.Narzędzia).dodaj(json.narzedzia);
        zasoby.get(TypProduktu.Jedzenie).dodaj(json.jedzenie);
        zasoby.get(TypProduktu.Ubrania).dodaj(json.ubrania);
        zasoby.get(TypProduktu.Diamenty).dodaj(json.diamenty);

        return zasoby;
    }

    @ToJson
    ZasobyJson toJsonZasoby(Map<TypProduktu, ZbiórProduktówInterface> zasoby) {
        ZasobyJson zasobyJson = new ZasobyJson();
        zasobyJson.diamenty = zasoby.get(TypProduktu.Diamenty).ilość();
        zasobyJson.jedzenie = zasoby.get(TypProduktu.Jedzenie).ilość();
        zasobyJson.narzedzia = zasoby.get(TypProduktu.Narzędzia).ilość();
        zasobyJson.programy = zasoby.get(TypProduktu.ProgramyKomputerowe).ilość();
        zasobyJson.ubrania = zasoby.get(TypProduktu.Ubrania).ilość();

        return zasobyJson;
    }

    @FromJson
    Map<TypProduktu, Integer> fromJsonProduktywność(ZasobyJson json) {
        EnumMap<TypProduktu, Integer> produktywność = new EnumMap<>(TypProduktu.class);
        produktywność.put(TypProduktu.Diamenty, json.diamenty);
        produktywność.put(TypProduktu.Jedzenie, json.jedzenie);
        produktywność.put(TypProduktu.Ubrania, json.ubrania);
        produktywność.put(TypProduktu.Narzędzia, json.narzedzia);
        produktywność.put(TypProduktu.ProgramyKomputerowe, json.programy);

        return produktywność;
    }

    @ToJson
    ZasobyJson toJsonProduktywność(Map<TypProduktu, Integer> produktywność) {
        ZasobyJson zasobyJson = new ZasobyJson();
        zasobyJson.diamenty = produktywność.get(TypProduktu.Diamenty);
        zasobyJson.jedzenie = produktywność.get(TypProduktu.Jedzenie);
        zasobyJson.narzedzia = produktywność.get(TypProduktu.Narzędzia);
        zasobyJson.programy = produktywność.get(TypProduktu.ProgramyKomputerowe);
        zasobyJson.ubrania = produktywność.get(TypProduktu.Ubrania);

        return zasobyJson;
    }

    @FromJson
    TypProduktu fromJasonKariera(String kariera) {
        switch (kariera) {
            case "programista":
                return TypProduktu.ProgramyKomputerowe;
            case "rolnik":
                return TypProduktu.Jedzenie;
            case "rzemieslnik":
                return TypProduktu.Ubrania;
            case "inzynier":
                return TypProduktu.Narzędzia;
            case "gornik":
                return TypProduktu.Diamenty;
            default:
                throw new JsonDataException();
        }
    }

    @ToJson
    String toJasonKariera(TypProduktu kariera) {
        switch (kariera) {
            case ProgramyKomputerowe:
                return "programista";
            case Jedzenie:
                return "rolnik";
            case Ubrania:
                return "rzemieslnik";
            case Narzędzia:
                return "inzynier";
            case Diamenty:
                return "gornik";
            default:
                throw new IllegalArgumentException();
        }
    }

//    @FromJson
//    Student fromJson(StudentJson json) {
//        return new Student(json.okres, json.zapas);
//    }
//
//    @ToJson
//    StudentJson toJson(Student student) {
//        StudentJson studentJson = new StudentJson();
//        studentJson.typ = "student";
//        studentJson.okres = student.okres();
//        studentJson.zapas = student.zapas();
//
//        return studentJson;
//    }
//
//    @FromJson
//    Okresowy fromJson(OkresowyJson json) {
//        return new Okresowy(json.okresowosc_nauki);
//    }
//
//    @ToJson
//    OkresowyJson toJson(Okresowy okresowy) {
//        OkresowyJson okresowyJson = new OkresowyJson();
//        okresowyJson.typ = "okresowy";
//        okresowyJson.okresowosc_nauki = okresowy.okresowośćNauki();
//
//        return okresowyJson;
//    }
//
//    @FromJson
//    Oszczędny fromJson(OszczędnyJson json) {
//        return new Oszczędny(json.limit_diamentow);
//    }
//
//    @ToJson
//    OszczędnyJson toJson(Oszczędny oszczędny) {
//        OszczędnyJson oszczędnyJson = new OszczędnyJson();
//        oszczędnyJson.typ = "oszczedny";
//        oszczędnyJson.limit_diamentow = oszczędny.limitDiamentów();
//
//        return oszczędnyJson;
//    }
//
//    @FromJson
//    Pracuś fromJson(PracuśJson json) {
//        return new Pracuś();
//    }
//
//    @ToJson
//    PracuśJson toJson(Pracuś pracuś) {
//        PracuśJson pracuśJson = new PracuśJson();
//        pracuśJson.typ = "pracus";
//
//        return pracuśJson;
//    }
//
//    @FromJson
//    Rozkładowy fromJson(RozkładowyJson json) {
//        return new Rozkładowy();
//    }
//
//    @ToJson
//    RozkładowyJson toJson(Rozkładowy rozkładowy) {
//        RozkładowyJson rozkładowyJson = new RozkładowyJson();
//        rozkładowyJson.typ = "rozkladowy";
//
//        return rozkładowyJson;
//    }
}
