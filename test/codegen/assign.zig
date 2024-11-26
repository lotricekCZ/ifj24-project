// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//0
//0
// --- EXPECTED OUTPUT ---
//
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    var a: i32 = 5.0;
    var b: f64 = 3;
    ifj.write(a);
}
