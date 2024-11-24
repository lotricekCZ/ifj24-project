// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//5
//0
// --- EXPECTED OUTPUT ---
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    const ifik = ifj.readi32();
    const ifik_nn = 1;
    if (ifik) |ifik_nn| {
        ifj.write(ifik_nn);
    } else {
        ifj.write("null");
    }
}
