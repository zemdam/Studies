package symulacja.zasób;

import static java.lang.Math.min;

public class ZbiórProduktów implements ZbiórProduktówInterface {
    private final Produkt produkt;

    public ZbiórProduktów(TypProduktu typProduktu) {
        produkt = new Produkt(typProduktu, 0);
    }

    @Override
    public void dodaj(Produkt produkt) {
        this.produkt.zwiększIlość(produkt.ilość());
        produkt.wyzerujIlość();
    }

    @Override
    public int ilość() {
        return produkt.ilość();
    }

    @Override
    public void zmniejszIlość(int oIle) {
        oIle = min(oIle, ilość());
        produkt.zmniejszIlość(oIle);
    }

    @Override
    public void zmniejszPoziom(int liczbaPrzedmiotów) {
    }

    @Override
    public int sumujPoziom() {
        return 0;
    }

    @Override
    public void usuń() {
        produkt.wyzerujIlość();
    }

    @Override
    public Produkt produktZNajwyższymPoziomem() {
        return produkt;
    }

    @Override
    public void zmniejszIlośćProduktuNajwyższegoPoziomu(int oIle) {
        zmniejszPoziom(oIle);
    }

    @Override
    public Produkt[] produkty() {
        Produkt[] produkty = new Produkt[1];
        produkty[0] = produkt;
        return produkty;
    }

    @Override
    public void usuńPusteProdukty() {
    }

    @Override
    public void dodaj(int ilość) {
        produkt.zwiększIlość(ilość);
    }
}
