const ifj = @import("ifj24.zig");

pub fn main() void {
    ifj.write((0 - 19) / (0 - 5));
    ifj.write("\n");
    ifj.write(19.0 / 5.0);
    ifj.write("\n");

    const a = 5;
    const b = 3;
    var c = a + b;
    ifj.write(c);
}
