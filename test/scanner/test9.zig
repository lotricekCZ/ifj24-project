const ifj = @import("ifj24.zig");

pub fn main() void {
    var x = ifj.readi32();

    if (x != 0) {
        if (x != 0) {
            if (x != 0) {
                if (x != 0) {
                    ifj.write(x);
                }
            }
        }
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
