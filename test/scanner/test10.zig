const ifj = @import("ifj24.zig");

pub fn main() void {
    var a : []u8 = ifj.string("kolo");
    for(a)|id|{
        const b :i32 = @as(i32, id); 
    }
}
