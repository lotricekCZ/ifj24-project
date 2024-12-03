const ifj = @import("ifj24.zig");

pub fn main() void { //error 1 - chybi ukoncujici uvozovka
    const msg = "Hello;
    ifj.write(msg);
}
