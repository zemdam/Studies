package symulacja.zasób;

public interface ZbiórProduktówInterface {
    void dodaj(Produkt produkt);

    void dodaj(int ilość);

    int ilość();

    void zmniejszIlość(int oIle);

    void zmniejszPoziom(int liczbaPrzedmiotów);

    int sumujPoziom();

    void usuń();

    void zmniejszIlośćProduktuNajwyższegoPoziomu(int oIle);

    Produkt produktZNajwyższymPoziomem();

    Produkt[] produkty();

    void usuńPusteProdukty();
}
