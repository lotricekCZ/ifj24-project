const ifj = @import("ifj24.zig");

pub fn main(c: f64) void {
    var x = 5;
    x = 5 + 2;

    if(x) |l|{
        var o = 3 + 6;
        var b = o + 2;
    } 
    else{
        var t = x + x;
    }
    
}
