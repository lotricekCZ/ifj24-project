const ifj = @import("ifj24.zig");

pub fn main() void {
    ifj.write(add(1,add(2,4.0,8),5));
    ifj.write("\n");
}

pub fn add(a: i32, b: i32, c: i32) i32 {
    return a + b + c;
}  

