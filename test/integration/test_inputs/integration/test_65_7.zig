const ifj = @import("ifj24.zig");

pub fn main() void {
    // Nacteni 5 hodnot
    ifj.write("Zadejte 5 celych cisel pro vypocet statistik:\n");

    const val1 = ifj.readi32();
    const val2 = ifj.readi32();
    const val3 = ifj.readi32();
    const val4 = ifj.readi32();
    const val5 = ifj.readi32();

    // Kontrola spravnosti nacteni
    if (val1 == null) {
        ifj.write("Chyba pri nacitani prvni hodnoty.\n");
        return;
    } else {}
    if (val2 == null) {
        ifj.write("Chyba pri nacitani druhe hodnoty.\n");
        return;
    } else {}
    if (val3 == null) {
        ifj.write("Chyba pri nacitani treti hodnoty.\n");
        return;
    } else {}
    if (val4 == null) {
        ifj.write("Chyba pri nacitani ctvrte hodnoty.\n");
        return;
    } else {}
    if (val5 == null) {
        ifj.write("Chyba pri nacitani pate hodnoty.\n");
        return;
    } else {}

    // Promenne pro vypocty
    var sum: i32 = 0;
    var min: i32 = val1;
    var max: i32 = val1; // max -> max ?i32, val1 -> val1 ?i32

    // Vypocty
    sum = sum + val1;
    if (val1 < min) {
        min = val1;
    } else {}

    if (val1 > max) {
        max = val1;
    } else {}

    sum = sum + val2;
    if (val2 < min) {
        min = val2;
    } else {}

    if (val2 > max) {
        max = val2;
    } else {}

    sum = sum + val3;
    if (val3 < min) {
        min = val3;
    } else {}

    if (val3 > max) {
        max = val3;
    } else {}

    sum = sum + val4;
    if (val4 < min) {
        min = val4;
    } else {}

    if (val4 > max) {
        max = val4;
    } else {}

    sum = sum + val5;
    if (val5 < min) {
        min = val5;
    } else {}

    if (val5 > max) {
        max = val5;
    } else {}

    // Vypocet prumeru
    const temp: f64 = ifj.i2f(sum);
    const avg: f64 = temp / 5.0;

    // Vystup vysledku
    ifj.write("Prumer: ");
    ifj.write(avg);
    ifj.write("\n");
    ifj.write("Minimum: ");
    ifj.write(min);
    ifj.write("\n");
    ifj.write("Maximum: ");
    ifj.write(max);
    ifj.write("\n");
}
