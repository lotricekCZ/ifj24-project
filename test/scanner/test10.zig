const ifj = @import("ifj24.zig");

pub fn main() void {
    recursion(10);
}

pub fn recursion(n: i32) void {
    if (n == 0) {
        return;
    }
    recursion(n - 1);
}
