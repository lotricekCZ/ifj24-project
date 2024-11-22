// --- INPUT ---
//1
//1
//1
//1.0
//"a"
// --- EXPECTED EXIT CODES ---
//0  // ifj2024 exit code
//0  // interpreter exit code
// --- EXPECTED OUTPUT ---
//1
//0
//0x1p+0
//0x0p+0
//a
//b
//2
//1
//0
// --- TEST CODE ---
//panic: reached unreachable code
const ifj = @import("ifj24.zig");

pub fn main() void {
    const a: ?i32 = 1;
    var b = a orelse 0;
    ifj.write(b);
    b = a.?;
    ifj.write(b);

    const c: ?i32 = null;
    const d = c orelse 0;
    ifj.write(d);

    const e: ?f64 = 1.0;
    var f = e orelse 0.0;
    ifj.write(f);
    f = e.?;
    f = e.?;
    ifj.write(f);

    const g: ?f64 = null;
    const h = g orelse 0.0;
    ifj.write(h);

    const i: ?[]u8 = ifj.string("a");
    var j = i orelse ifj.string("b");
    ifj.write(j);
    j = i.?;
    ifj.write(j);

    const k: ?[]u8 = null;
    const l = k orelse ifj.string("b");
    ifj.write(l);

    const o = a orelse 0 + (c orelse 0) + a.? + c.?;
    ifj.write(o);

    var q = ifj.readi32() orelse 0;
    ifj.write(q);
    q = ifj.readi32() orelse 0;
    ifj.write(q);

    const s: ?i32 = ifj.readi32() orelse unreachable;
    ifj.write(s.?);
    const t: ?f64 = ifj.readf64() orelse unreachable;
    ifj.write(t.?);
    const u: ?[]u8 = ifj.readstr() orelse unreachable;
    ifj.write(u.?);

    //const r = ifj.readf64();

    //const p = r.?;
    //ifj.write(p);
}
