// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//4
//0
// --- EXPECTED OUTPUT ---
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    ifj.write(1, 2, 3);
}

pub fn add(a: i32, b: i32) i32 {
    _ = a + b;
    return 1;
}
