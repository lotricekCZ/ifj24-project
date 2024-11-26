// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//0  // ifj2024 exit code
//0  // interpreter exit code
// --- EXPECTED OUTPUT ---
//0x1.5133333333333p5
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    const a: f64 = 5.0;
    const b: f64 = 2.5;
    const c: f64 = 10.0;
    const d: f64 = 7.3;

    const result: f64 = ((a + b) * (c - 3) / 2.0) + (d * 3) - ((c + a) / b);
    ifj.write(result);
}
