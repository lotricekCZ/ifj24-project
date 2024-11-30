// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//0
//0
// --- EXPECTED OUTPUT ---
//Valid input
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    const col: []u8 = ifj.string("a");

    if (check_input(col) == 1) {
        ifj.write("Valid input");
    } else {
        ifj.write("Invalid input");
    }

    ifj.write("\n");
}

pub fn check_input(col: []u8) i32 {
    if (ifj.ord(col, 0) <= ifj.ord(ifj.string("h"), 0)) {
        return 1;
    } else {
        ifj.write("Col too big");
    }

    ifj.write("\n");

    return 0;
}
