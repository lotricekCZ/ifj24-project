// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//4
//0
// --- EXPECTED OUTPUT ---
//
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn func() i32 {
    return 3.2;
}

pub fn main() void {
    _ = func();
}
