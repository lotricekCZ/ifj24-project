// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//7
//0
// --- EXPECTED OUTPUT ---
//
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = true or false and 5 == 4 - 3 orelse 2 * !1.?;
}
