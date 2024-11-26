// --- INPUT ---
//1
//
//1
//1.0
//a
// --- EXPECTED EXIT CODES ---
//0  // ifj2024 exit code
//0  // interpreter exit code
// --- EXPECTED OUTPUT ---
//1
//1
//0
//0x1p0
//0x1p0
//0x0p0
//a
//a
//b
//3
//1
//0
//1
//0x1p0
//a
// --- TEST CODE ---
//panic: reached unreachable code
const ifj = @import("ifj24.zig");

pub fn main() void {
    const a: ?i32 = 1;
    var b = a orelse 0;
    ifj.write(b); //1
    ifj.write("\n");
    b = a.?;
    ifj.write(b); //1
    ifj.write("\n");

    const c: ?i32 = null;
    const d = c orelse 0;
    ifj.write(d); //0
    ifj.write("\n");

    const e: ?f64 = 1.0;
    var f = e orelse 0.0;
    ifj.write(f); //1.0
    ifj.write("\n");
    f = e.?;
    ifj.write(f); //1.0
    ifj.write("\n");

    const g: ?f64 = null;
    const h = g orelse 0.0;
    ifj.write(h); //0.0
    ifj.write("\n");

    const i: ?[]u8 = ifj.string("a");
    var j = i orelse ifj.string("b");
    ifj.write(j); //a
    ifj.write("\n");
    j = i.?;
    ifj.write(j); //a
    ifj.write("\n");

    const k: ?[]u8 = null;
    const l = k orelse ifj.string("b");
    ifj.write(l); //b
    ifj.write("\n");

    const o = a orelse 0 + (c orelse 0) + a.? + a.?;
    ifj.write(o); //3
    ifj.write("\n");

    var q = ifj.readi32() orelse 0;
    ifj.write(q); //1
    ifj.write("\n");
    q = ifj.readi32() orelse 0;
    ifj.write(q); //0
    ifj.write("\n");

    const s: i32 = ifj.readi32() orelse unreachable;
    ifj.write(s); //1
    ifj.write("\n");
    const t: f64 = ifj.readf64() orelse unreachable;
    ifj.write(t); //1.0
    ifj.write("\n");
    const u: []u8 = ifj.readstr() orelse unreachable;
    ifj.write(u); //a

    //const r = ifj.readf64();

    //const p = r.?;
    //ifj.write(p);
}
