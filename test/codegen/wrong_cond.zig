// --- INPUT ---
// --- EXPECTED EXIT CODES ---
//7  // ifj2024 exit code
//0  // interpreter exit code
// --- EXPECTED OUTPUT ---
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    if (1 + 1) {} else {}
}
