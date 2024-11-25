// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//2
//0
// --- EXPECTED OUTPUT ---
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = 1.0;
    const b = 2.0;
    const c = a b;
    ifj.write(c);
}
