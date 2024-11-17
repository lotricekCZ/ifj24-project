const ifj = @import("ifj24.zig");

pub fn main() void {
    var x = 85.0;

    while (x > 0.0) {
        x = x - 1.0;
        ifj.write(ifj.f2i(x));
        ifj.write("\n");
    }

    // x = ifj.readi32();
    // if (x == 2) {
    //     var o = 3 + 6;
    //     var b = o + 2;
    // } else {
    //     var t = x + x;
    //     ifj.write(t);
    //     ifj.write("\n");
    // }
}
