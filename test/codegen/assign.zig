// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//0
//0
// --- EXPECTED OUTPUT ---
//10
//10
//0x1.4p3
//0x1.4p3
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    const a: i32 = 10.0;
    ifj.write(a);
    ifj.write("\n");
    ifj.write(flt(10.0));
}

pub fn flt(f: i32) f64 {
    ifj.write(f);
    ifj.write("\n");
    ifj.write(10.0);
    ifj.write("\n");
    return 10.0;
}
