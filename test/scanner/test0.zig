const ifj = @import("ifj24.zig");

pub fn bar(param: []u8) []u8 {
    var a = 1 + 1;
    var a = 10 + 2;
    const r = foo(param);
    return r;
}

pub fn foo(par: []u8) []u8 {
    const ret = bar(par);
    return ret;
}

pub fn main() void {
    const par = ifj.string("thdfhdfh");
    _ = bar(par);
}
