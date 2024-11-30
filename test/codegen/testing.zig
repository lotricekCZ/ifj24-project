// --- INPUT ---
//a
// --- EXPECTED EXIT CODES ---
//0
//0
// --- EXPECTED OUTPUT ---
//a
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    ifj.write(ifj.readstr());
}
