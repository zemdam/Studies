package symulacja.zasób;

import java.util.LinkedList;
import java.util.ListIterator;

import static java.lang.Math.min;

public class ZbiórProduktówZPoziomem implements ZbiórProduktówInterface {

    private final TypProduktu typProduktu;
    private int łącznaIlość;
    private LinkedList<Produkt> produktyZPoziomami;

    public ZbiórProduktówZPoziomem(TypProduktu typProduktu) {
        łącznaIlość = 0;
        produktyZPoziomami = new LinkedList<>();
        this.typProduktu = typProduktu;
    }

    @Override
    public void dodaj(Produkt produkt) {
        łącznaIlość += produkt.ilość();
        produktyZPoziomami.add(new ProduktZPoziomem(produkt));
    }

    @Override
    public void zmniejszPoziom(int liczbaPrzedmiotów) {
        liczbaPrzedmiotów = min(ilość(), liczbaPrzedmiotów);
        ListIterator<Produkt> iterator = produktyZPoziomami.listIterator();
        Produkt aktualne;

        while (iterator.hasNext() && liczbaPrzedmiotów > 0) {
            aktualne = iterator.next();

            if (aktualne.ilość() > liczbaPrzedmiotów) {
                aktualne.zmniejszIlość(liczbaPrzedmiotów);

                if (aktualne.poziom() > 1) {
                    iterator.add(new ProduktZPoziomem(aktualne.typProduktu(), liczbaPrzedmiotów,
                            aktualne.poziom() - 1));
                } else {
                    łącznaIlość -= liczbaPrzedmiotów;
                }

                return;
            }

            liczbaPrzedmiotów -= aktualne.ilość();
            aktualne.zmniejszPoziom();

            if (aktualne.poziom() == 0) {
                łącznaIlość -= aktualne.ilość();
                iterator.remove();
            }
        }
    }

    @Override
    public int sumujPoziom() {
        ListIterator<Produkt> iterator = produktyZPoziomami.listIterator();
        Produkt aktualne;
        int sumaPoziom = 0;

        while (iterator.hasNext()) {
            aktualne = iterator.next();
            sumaPoziom += aktualne.poziom() * aktualne.ilość();
        }

        return sumaPoziom;
    }

    @Override
    public void usuń() {
        łącznaIlość = 0;
        produktyZPoziomami = new LinkedList<>();
    }

    @Override
    public void zmniejszIlość(int oIle) {
        oIle = min(oIle, ilość());
        łącznaIlość -= oIle;
        Produkt aktualne;
        ListIterator<Produkt> iterator = produktyZPoziomami.listIterator();
        while (oIle > 0 && iterator.hasNext()) {
            aktualne = iterator.next();
            if (aktualne.ilość() > oIle) {
                aktualne.zmniejszIlość(oIle);
                return;
            }
            oIle -= aktualne.ilość();
            iterator.remove();
        }
    }

    @Override
    public void zmniejszIlośćProduktuNajwyższegoPoziomu(int oIle) {
        Produkt produkt = znajdźNajwyższyPoziom();
        if (produkt == null) return;
        produkt.zmniejszIlość(oIle);

        if (produkt.ilość() == 0) {
            produktyZPoziomami.remove(produkt);
        }
    }

    @Override
    public int ilość() {
        return łącznaIlość;
    }

    private Produkt znajdźNajwyższyPoziom() {
        if (produktyZPoziomami.size() == 0) {
            return null;
        }
        ListIterator<Produkt> iterator = produktyZPoziomami.listIterator();
        Produkt produkt = iterator.next();
        Produkt aktualny;

        while (iterator.hasNext()) {
            aktualny = iterator.next();
            if (aktualny.poziom() > produkt.poziom()) {
                produkt = aktualny;
            }
        }

        return produkt;
    }

    @Override
    public Produkt produktZNajwyższymPoziomem() {
        return znajdźNajwyższyPoziom();
    }

    @Override
    public Produkt[] produkty() {
        Produkt[] produkty = new Produkt[produktyZPoziomami.size()];
        return produktyZPoziomami.toArray(produkty);
    }

    @Override
    public void usuńPusteProdukty() {
        produktyZPoziomami.removeIf(produkt -> produkt.ilość() == 0);
    }

    @Override
    public void dodaj(int ilość) {
        łącznaIlość += ilość;
        produktyZPoziomami.add(new ProduktZPoziomem(typProduktu, ilość, 1));
    }
}
