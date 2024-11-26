// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//7  // ifj2024 exit code
//0  // interpreter exit code
// --- EXPECTED OUTPUT ---
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    const a: i32 = 5;
    var b: ?f64 = ifj.readf64();
    if (b) |B| {
        const ab: f64 = a + B; //konstanty se mohou retypovat
        ifj.write(ab);
    } else {
        b = 5.2;
    }
}
