const ifj = @import("ifj24.zig");

pub fn main() void {
    const str = "\xIFJ"; //error 1 - nevalidni hexadecimalni cislice
    ifj.write(str);
}
