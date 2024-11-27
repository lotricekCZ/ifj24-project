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
    ifj.write(ifj.substring(ifj.string("Hello, World!"), 0, 0).?);
}
