// --- INPUT ---
//ahoj
// --- EXPECTED EXIT CODES ---
//0
//0
// --- EXPECTED OUTPUT ---
//ahoj
//Sring and length: --after7
//before--ahoj--after
//0
//bc
//abeceda: ABCDEFGHIJKLMNOPQRSTUVWXYZ
//97
//104
//111
//106
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = ifj.readstr();
    var a_with_no_null: []u8 = ifj.string("");
    if (a) |b| {
        a_with_no_null = b;
    } else {}
    ifj.write(a_with_no_null);
    ifj.write("\n");
    const str_after = ifj.string("--after");
    const str_before = ifj.string("before--");

    const len_after = ifj.length(str_after);
    ifj.write(ifj.concat(ifj.string("Sring and length: "), str_after));
    ifj.write(len_after);
    ifj.write("\n");

    var cc = ifj.concat(ifj.concat(str_before, a_with_no_null), str_after);
    ifj.write(cc);
    ifj.write("\n");

    cc = ifj.concat(ifj.string(""), ifj.string(""));
    ifj.write(cc);
    ifj.write(ifj.length(cc));
    ifj.write("\n");

    const t: []u8 = ifj.string("abcdef");
    const sub = ifj.substring(t, 1, 3);
    if (sub) |sub_without_null| {
        ifj.write(ifj.concat(sub_without_null, ifj.string("\n")));
    } else {}

    ifj.write("abeceda: ");

    var i: i32 = 65;
    while (90 >= i) {
        const j = i;
        ifj.write(ifj.chr(j));
        i = i + 1;
    }
    ifj.write("\n");

    const a_len = ifj.length(a_with_no_null);
    var j: i32 = 0;
    while (a_len > j) {
        ifj.write(ifj.ord(a_with_no_null, j));
        ifj.write("\n");
        j = j + 1;
    }
}
