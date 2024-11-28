// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//5
//0
// --- EXPECTED OUTPUT ---
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = 5;
    a = 10 + 4;
    ifj.write(a);
}
