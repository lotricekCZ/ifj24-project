const ifj = @import("ifj24.zig");

pub fn main() void {
    const a :?i32 = 9;
    const b :?i32 = 8;
    _ = a.? + b.?;
}
