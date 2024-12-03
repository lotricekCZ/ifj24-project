const ifj = @import("ifj24.zig");

pub fn bar() void {
    return;
}

pub fn main() void {
    bar() = 50; // Cannot assign to the return value of a function
}
