// --- INPUT ---
// --- EXPECTED EXIT CODES ---
//5  // ifj2024 exit code
//0  // interpreter exit code
// --- EXPECTED OUTPUT ---
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = 5;
    if (1 == 1) {
        var a = 6;
        a = 7;
        _ = a;
    } else {}
    _ = a;
}
