// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//0
//0
// --- EXPECTED OUTPUT ---
//566
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    var a: i32 = 3;
    while (a != 0) {
        a = a - 1;
        if (a == 2) {
            const b: i32 = 5;
            ifj.write(b);
        } else {
            var b: i32 = 4;
            b = 6;
            ifj.write(b);
        }
    }
}
