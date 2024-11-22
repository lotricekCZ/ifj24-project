// --- INPUT ---
//1
//2
//3
//4
//
// --- EXPECTED EXIT CODES ---
//0  // ifj2024 exit code
//0  // interpreter exit code
// --- EXPECTED OUTPUT ---
//OK
//OK,OK,OK,OK,
//OK,OK,OK,
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    _ = 1 + 1;
    _ = 1 - 1;
    _ = 1 * 1;
    _ = 1 / 1;
    _ = 1 == 1;
    _ = 1 != 1;
    _ = 1 < 1;
    _ = 1 <= 1;
    _ = 1 > 1;
    _ = 1 >= 1;

    const a: ?i32 = 1;
    if (a) |_| {
        ifj.write("OK\n");
    }

    var b: ?i32 = ifj.readi32();
    while (b) |_| {
        ifj.write("OK,");
        b = ifj.readi32();
    }
    ifj.write("\n");

    const c = ifj.string("123");
    for (c) |_| {
        ifj.write("OK,");
    }
}
