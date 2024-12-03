const ifj = @import("ifj24.zig");

pub fn getNumber() i32 {
    const a = 2;
    ifj.write(a);
    // Missing a return statement for the other branch
}

pub fn main() void {}
