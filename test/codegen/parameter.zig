// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//5
//0
// --- EXPECTED OUTPUT ---
//
// --- TEST CODE ---
//Popravde nevim proc ma byt navratovy kod 5, ale tak to bylo v tech testech
const ifj = @import("ifj24.zig");

pub fn func(x: i32) i32 {
    x = 5;
    return x;
}

pub fn main() void {
    _ = func(5);
}
