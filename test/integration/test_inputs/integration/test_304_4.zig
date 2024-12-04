const ifj = @import("ifj24.zig");

pub fn main() void {
   var str: []u8 = ifj.concat(123, "World");
}
