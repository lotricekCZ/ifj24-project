// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//0  // ifj2024 exit code
//0  // interpreter exit code
// --- EXPECTED OUTPUT ---
//
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = ifj.string(
        \\;;""\n\\\x60
        \\Nic
    );
    ifj.write(a);

    const b = ifj.string(
        \\Nahore
        //komentar
        \\Dole
    );
    ifj.write(b);

    ifj.write(
                \\Tohle je
        \\novy radek
    );

    for (
        \\a
        \\b
    ) |q| {
        ifj.write(q);
    }

    ifj.write(two_prints(ifj.string(
        \\quack
        \\quack
    ), ifj.string(
        \\quacky
        \\quacky
    )));

      const s1 : []u8 = ifj.string( 
    \\Toto 
    \\ je 
    \\
    \\ nejaky 
                        \\ text  // ve viceradkovem retezcovem literalu nelze mit komentar
    ); // ukoncujici uvozovky ovlivnuji implicitni odsazeni vnitrnich radku retezce
  ifj.write(s1);
}

pub fn print(s: []u8) void {
    ifj.write(s);
}

pub fn two_prints(s: []u8, z: []u8) []u8 {
    ifj.write(s);
    ifj.write(z);
    return ifj.string(
        \\re
        \\turn
    );
}
