// --- INPUT ---
//5
//-8
//9
//10
//0
//ahoj
// --- EXPECTED EXIT CODES ---
//0
//0
// --- EXPECTED OUTPUT ---
//Vitaj v komplexnom testovacom programe pre jazyk IFJ24!
//Sucet cisel: 55
//Vysledok je vacsi ako 50.
//Vypis cisel od 1 do 5:
//1
//2
//3
//4
//5
//Faktorial cisla 5 je 120.
//Spojeny retazec: Testovanie prace s retazcami. IFJ24 je super!
//Zadajte cisla na scitanie (0 ukonci vstup):
//Sucet zadanych cisel je: 16
//Zadajte 'ahoj', aby ste pokracovali:
//Zadany retazec je spravny!
//Testovaci program ukonceny uspesne.
// --- TEST CODE ---
const ifj = @import("ifj24.zig");
// Hlavna funkcia programu
pub fn main() void {
    ifj.write("Vitaj v komplexnom testovacom programe pre jazyk IFJ24!\n");

    // Praca s cislami a zakladnymi operaciami
    const a: i32 = 42;
    const b: i32 = 13;
    var result: i32 = a + b;
    ifj.write("Sucet cisel: ");
    ifj.write(result);
    ifj.write("\n");

    // Praca s podmienkami
    if (result > 50) {
        result = 10;
        ifj.write("Vysledok je vacsi ako 50.\n");
    } else {
        ifj.write("Vysledok je mensi alebo rovny 50.\n");
    }

    // Praca s cyklom while
    ifj.write("Vypis cisel od 1 do 5:\n");
    var i: i32 = 1;
    while (i <= 5) {
        ifj.write(i);
        ifj.write("\n");
        i = i + 1;
    }

    // Praca s funkciami (faktorial - rekurzivne)
    const factorialInput: i32 = 5;
    const factorialResult = factorial(factorialInput);
    ifj.write("Faktorial cisla ");
    ifj.write(factorialInput);
    ifj.write(" je ");
    ifj.write(factorialResult);
    ifj.write(".\n");

    // Praca s retazcami
    const str1 = ifj.string("Testovanie prace s retazcami.");
    const str2 = ifj.string(" IFJ24 je super!");
    const combined = ifj.concat(str1, str2);
    ifj.write("Spojeny retazec: ");
    ifj.write(combined);
    ifj.write("\n");

    // Cyklus while na citanie vstupu
    var sum: i32 = 0;
    ifj.write("Zadajte cisla na scitanie (0 ukonci vstup):\n");

    var br: i32 = 1;
    while (br != 0) {
        const input = ifj.readi32();
        if (input) |num| {
            if (num == 0) {
                br = 0;
            } else {
                sum = sum + num;
            }
        } else {
            ifj.write("Neplatny vstup! Skuste znova.\n");
        }
    }
    ifj.write("Sucet zadanych cisel je: ");
    ifj.write(sum);
    ifj.write("\n");

    // Praca s porovnavanim retazcov
    ifj.write("Zadajte 'ahoj', aby ste pokracovali:\n");
    const inputStr = ifj.readstr();
    const expected = ifj.string("ahoj");
    if (inputStr) |in_str| {
        if (ifj.strcmp(in_str, expected) == 0) {
            ifj.write("Zadany retazec je spravny!\n");
        } else {
            ifj.write("Nespravny retazec. Koncim program. 1\n");
            return;
        }
    } else {
        ifj.write("Nespravny retazec. Koncim program. 2\n");
        return;
    }

    ifj.write("Testovaci program ukonceny uspesne.\n");
}

// Rekurzivna funkcia na vypocet faktorialu
pub fn factorial(n: i32) i32 {
    if (n <= 1) {
        return 1;
    } else {}
    return n * factorial(n - 1);
}
