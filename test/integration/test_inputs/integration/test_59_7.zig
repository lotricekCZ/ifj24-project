const ifj = @import("ifj24.zig");

pub fn multiply(a: i32, b: i32) i32 {
    return a * b;
}

pub fn main() void {
    const x: i32 = "hello"; //error 7 - typova kompatibilita
    const y = 3;
    const result = multiply(x, y);
    ifj.write("The product is: ");
    ifj.write(result);
}
