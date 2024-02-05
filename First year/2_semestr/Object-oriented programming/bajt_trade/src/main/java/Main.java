import com.squareup.moshi.JsonAdapter;
import com.squareup.moshi.Moshi;
import com.squareup.moshi.adapters.PolymorphicJsonAdapterFactory;
import symulacja.Symulacja;
import symulacja.adapter.SymulacjaAdapter;
import symulacja.agent.robotnik.strategia.kupowanie.*;
import symulacja.agent.robotnik.strategia.produkcja.*;
import symulacja.agent.robotnik.strategia.uczenie.*;
import symulacja.agent.spekulant.Regulujący;
import symulacja.agent.spekulant.Spekulant;
import symulacja.agent.spekulant.Wypukły;
import symulacja.agent.spekulant.Średni;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        if (args.length != 2) {
            throw new RuntimeException();
        }
        File wejście = new File(args[0]);
        Scanner skaner;

        try {
            skaner = new Scanner(wejście);
        } catch (FileNotFoundException e) {
            throw new RuntimeException(e);
        }

        StringBuilder builder = new StringBuilder();

        while (skaner.hasNextLine()) {
            builder.append(skaner.nextLine());
        }

        String j = builder.toString();
        Moshi moshi =
                new Moshi.Builder().add(new SymulacjaAdapter()).add(PolymorphicJsonAdapterFactory.of(StrategiaNauki.class, "typ")
                        .withSubtype(Student.class, "student").withSubtype(Rozkładowy.class, "rozkladowy")
                        .withSubtype(Pracuś.class, "pracus").withSubtype(Oszczędny.class, "oszczedny")
                        .withSubtype(Okresowy.class, "okresowy")).add(PolymorphicJsonAdapterFactory.of(StrategiaKupna.class, "typ")
                        .withSubtype(Czyścioszek.class, "czyscioszek").withSubtype(Gadżeciarz.class, "gadzeciarz")
                        .withSubtype(Technofob.class, "technofob").withSubtype(Zmechanizowany.class, "zmechanizowany"))
                .add(PolymorphicJsonAdapterFactory.of(StrategiaProdukcji.class, "typ").withSubtype(Chciwy.class,
                                "chciwy")
                        .withSubtype(Krótkowzroczny.class, "krotkowzroczny")
                        .withSubtype(Losowy.class, "losowy")
                        .withSubtype(Perspektywiczny.class, "perspektywiczny")
                        .withSubtype(Średniak.class, "sredniak")).add(PolymorphicJsonAdapterFactory.of(Spekulant.class, "kariera")
                        .withSubtype(Regulujący.class, "regulujacy_rynek")
                        .withSubtype(Wypukły.class, "wypukly")
                        .withSubtype(Średni.class, "sredni")).build();

        JsonAdapter<Symulacja> adapter = moshi.adapter(Symulacja.class);
        Symulacja symulacja;

        try {
            symulacja = adapter.fromJson(j);
            symulacja.przeprowadźSymulację();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        try {
            FileWriter writer = new FileWriter(args[1]);
            writer.write(adapter.indent("\t").toJson(symulacja));
            writer.close();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }


//        Zasoby zasoby = new Zasoby();
//        zasoby.dodajProdukt(new Produkt(TypProduktu.Diamenty, 100));
//        zasoby.dodajProdukt(new Produkt(TypProduktu.Ubrania, 100));
//        zasoby.dodajProdukt(new Produkt(TypProduktu.Narzędzia, 100));
//        zasoby.dodajProdukt(new Produkt(TypProduktu.Jedzenie, 100));
//        zasoby.dodajProdukt(new Produkt(TypProduktu.ProgramyKomputerowe, 100));
//
//        EnumMap<TypProduktu, Integer> produktywność = new EnumMap<>(TypProduktu.class);
//        for (TypProduktu typ : TypProduktu.values()) {
//            produktywność.put(typ, 100);
//        }
//
//        StrategiaNauki nauka = new Student(5, 2);
//        StrategiaKariery kariera = new Rewolucjonista();
//        StrategiaKupna kupno = new Gadżeciarz(100);
//        StrategiaProdukcji produkcja = new Chciwy();
//        ŚcieżkaKariery ścieżka = new ŚcieżkaKariery(TypProduktu.ProgramyKomputerowe);
//
//        Robotnik robotnik = new Robotnik(zasoby, 1, produktywność, nauka, kariera, kupno, produkcja, ścieżka);
//        ArrayList<Robotnik> robotnicy = new ArrayList<>();
//        robotnicy.add(robotnik);
//
//        Zasoby zasoby2 = new Zasoby();
//        zasoby.dodajProdukt(new Produkt(TypProduktu.Diamenty, 100));
//        zasoby.dodajProdukt(new Produkt(TypProduktu.Ubrania, 100));
//        zasoby.dodajProdukt(new Produkt(TypProduktu.Narzędzia, 100));
//        zasoby.dodajProdukt(new Produkt(TypProduktu.Jedzenie, 100));
//        zasoby.dodajProdukt(new Produkt(TypProduktu.ProgramyKomputerowe, 100));
//
//        Spekulant spekulant = new Wypukły(zasoby2, 12);
//        ArrayList<Spekulant> spekulanci = new ArrayList<>();
//
//        ŚredniaCena cena = new ŚredniaCena();
//        cena.dodajCenę(TypProduktu.ProgramyKomputerowe, 1, 10);
//        cena.dodajCenę(TypProduktu.Jedzenie, 1, 125.27);
//        cena.dodajCenę(TypProduktu.Ubrania, 1, 10);
//        cena.dodajCenę(TypProduktu.Narzędzia, 1, 105);
//
//        Giełda symulacja.giełda = new Socjalistyczna(robotnicy, spekulanci, cena, 2, 5);
//        giełda.przeprowadźGiełdę();
    }
}
