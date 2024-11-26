// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//7
//0
// --- EXPECTED OUTPUT ---
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    const b : ?i32 = null;
    const a = true or false and 5 == 4 - 3 orelse 2 * !b.?;
}
