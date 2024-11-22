const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = 5;
    const b = 3;
    var c = a + b;
    ifj.write(c);
}
