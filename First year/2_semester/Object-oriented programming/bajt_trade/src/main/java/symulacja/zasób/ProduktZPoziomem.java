package symulacja.zasób;

public class ProduktZPoziomem extends Produkt {
    private int poziom;

    public ProduktZPoziomem(TypProduktu typProduktu, int ilość, int poziom) {
        super(typProduktu, ilość);
        this.poziom = poziom;
    }

    public ProduktZPoziomem(Produkt produkt) {
        super(produkt);
        poziom = produkt.poziom();
    }

    @Override
    public void zmniejszPoziom() {
        poziom--;
    }

    @Override
    public int poziom() {
        return poziom;
    }

    @Override
    public Produkt zmniejszonyProdukt(int wartość) {
        zmniejszIlość(wartość);

        return new ProduktZPoziomem(typProduktu(), ilość(), poziom);
    }
}
