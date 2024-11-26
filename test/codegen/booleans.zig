// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//0
//0
// --- EXPECTED OUTPUT ---
//true
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = (true == false) == false;

    ifj.write(a);
}
