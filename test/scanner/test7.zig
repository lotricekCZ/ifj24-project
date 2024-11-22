const ifj = @import("ifj24.zig");

// pub fn main() void {
//     var a: ?[]u8 = null;
//     ifj.write(add(1, ifj.f2i(1.0)));
// }

pub fn main() void {
    var a: ?[]u8 = null;
    if (a) |_| {
        //var a = ifj.strcmp(s, "12");
        var d = 2;
        if (d == 0) {
            ifj.write(d);
            ifj.write("\n");
        } else {
            ifj.write("blbost");
        }
    } else if (1 == 1) ifj.write("jou");
    ifj.write("\n");
    //ifj.write(add(1, 2, 3), add(1, 2, 3));
    ifj.write(add(add(3, 2, 1), add(3, add(2, 1, 1), add(4, 1, 1)), add(5, 1, 1)));
    ifj.write("\n");
}

pub fn add(a: i32, b: i32, c: i32) i32 {
    return a * b / c;
}
