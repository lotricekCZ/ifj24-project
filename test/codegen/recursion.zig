// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//0  // ifj2024 exit code
//0  // interpreter exit code
// --- EXPECTED OUTPUT ---
//9876543210123456789
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    recursion(9);
}

pub fn recursion(n: i32) void {
    ifj.write(n);
    if (n == 0) {
        return;
    }
    recursion(n - 1);
    ifj.write(n);
}
