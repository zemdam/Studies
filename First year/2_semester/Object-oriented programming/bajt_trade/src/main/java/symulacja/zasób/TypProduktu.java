package symulacja.zasób;

public enum TypProduktu {
    Jedzenie(false, true),
    Ubrania(true, true),
    Narzędzia(true, true),
    Diamenty(false, false),
    ProgramyKomputerowe(true, true);

    private final boolean czyZPoziomem;
    private final boolean czyNaSprzedaż;

    TypProduktu(boolean czyZPoziomem, boolean czyNaSprzedaż) {
        this.czyZPoziomem = czyZPoziomem;
        this.czyNaSprzedaż = czyNaSprzedaż;
    }

    public boolean czyZPoziomem() {
        return czyZPoziomem;
    }

    public boolean czyNaSprzedaż() {
        return czyNaSprzedaż;
    }
}
