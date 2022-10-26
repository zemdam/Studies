package symulacja.agent;

import com.squareup.moshi.Json;
import org.jetbrains.annotations.NotNull;
import symulacja.Info;
import symulacja.zasób.Produkt;
import symulacja.zasób.TypProduktu;
import symulacja.zasób.ZbiórProduktówInterface;

import java.util.EnumMap;
import java.util.Map;

abstract public class Agent implements Comparable<Agent> {
    private final int id;
    protected Map<TypProduktu, ZbiórProduktówInterface> zasoby;
    @Json(ignore = true)
    protected double liczbaDiamentów;

    protected Agent() {
        zasoby = new EnumMap<>(TypProduktu.class);
        id = 0;
    }

    protected Agent(EnumMap<TypProduktu, ZbiórProduktówInterface> zasoby, int id) {
        this.zasoby = zasoby;
        this.id = id;
    }

    public void zwiększLiczbęDiamentów(double wartość) {
        liczbaDiamentów += wartość;
    }

    public void zmniejszLiczbęDiamentów(double wartość) {
        liczbaDiamentów -= wartość;
    }

    public int id() {
        return id;
    }

    @Override
    public int compareTo(@NotNull Agent o) {
        if (liczbaDiamentów < o.liczbaDiamentów) {
            return 1;
        } else if (liczbaDiamentów > o.liczbaDiamentów) {
            return -1;
        }

        if (id > o.id) {
            return 1;
        }

        return -1;
    }

    public int ile(TypProduktu typProduktu) {
        return zasoby.get(typProduktu).ilość();
    }

    protected void dodajProdukt(Produkt produkt) {
        zasoby.get(produkt.typProduktu()).dodaj(produkt);
    }

    protected void wyczyść() {
        for (TypProduktu typ : TypProduktu.values()) {
            zasoby.get(typ).usuńPusteProdukty();
        }
    }

    protected Produkt[] dajProdukty(TypProduktu typProduktu) {
        return zasoby.get(typProduktu).produkty();
    }

    public abstract void spędźDzień(Info info);
}
