// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//4
//0
// --- EXPECTED OUTPUT ---
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    print();
}

pub fn print() []u8 {
    return ifj.chr(10);
}
