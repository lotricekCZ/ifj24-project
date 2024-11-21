// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//0  // ifj2024 exit code
//0  // interpreter exit code
// --- EXPECTED OUTPUT ---
//1
//3
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    ifj.write(num());

    ifj.write(add(1, 2));
}

pub fn num() i32 {
    return 1;
}

pub fn add(a: i32, b: i32) i32 {
    return a + b;
}
