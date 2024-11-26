// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//0  // ifj2024 exit code
//0  // interpreter exit code
// --- EXPECTED OUTPUT ---
//true
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    const a: ?i32 = 5;
    const b = true;
    ifj.write(a.? == 5 and !!b);
}
