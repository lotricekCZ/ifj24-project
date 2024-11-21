// --- INPUT ---
//1
//1.0
//a
// --- EXPECTED EXIT CODES ---
//0  // ifj2024 exit code
//0  // interpreter exit code
// --- EXPECTED OUTPUT ---
//1
//0x1p0
//a
//string
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = ifj.readi32();
    if (a) |write| {
        ifj.write(write);
    }
    ifj.write("\n");

    const b = ifj.readf64();
    if (b) |write| {
        ifj.write(write);
    }
    ifj.write("\n");

    const c = ifj.readstr();
    if (c) |write| {
        ifj.write(write);
    }
    ifj.write("\n");

    const d = ifj.string("string\n");
    ifj.write(d);
    ifj.write("\n");

    // const e: []u8 = ifj.concat(ifj.string("123"), ifj.string("456\n"));
    // ifj.write(e);
    // ifj.write("\n");

    // const f = ifj.strcmp(ifj.string("Stejne"), ifj.string("Stejne"));
    // ifj.write(f);
    // ifj.write("\n");

    // const g = ifj.strcmp(ifj.string("Vetsi"), ifj.string("Mensi"));
    // ifj.write(g);
    // ifj.write("\n");

    // const h = ifj.strcmp(ifj.string("Mensi"), ifj.string("Vetsi"));
    // ifj.write(h);
    // ifj.write("\n");

    // //const i = ifj.substring("Slovo", 0, 2);
    // //if (i) |write| {
    // //    ifj.write(write);
    // //}
    // ifj.write("\n");

    // const j = ifj.ord(ifj.string("Slovo"), 0);
    // ifj.write(j);
    // ifj.write("\n");

    // const k = ifj.chr(97);
    // ifj.write(k);
    // ifj.write("\n");

    // const l = ifj.i2f(1);
    // ifj.write(l);
    // ifj.write("\n");

    // const m = ifj.f2i(1.0);
    // ifj.write(m);
    // ifj.write("\n");

    // const n = ifj.length(ifj.string("Slovo"));
    // ifj.write(n);
    // ifj.write("\n");
}
