const ifj = @import("ifj24.zig");

pub fn main() void {
    ifj.write(add(add(3, 2, 1), add(3, add(1,9,7), 5), 5));
    ifj.write("\n");
}

pub fn add(a: i32, b: i32, c: i32) i32 {
    return a + b + c;
}