const ifj = @import("ifj24.zig");

pub fn main() void {
    const str = \\xAG; //error 1 nevalidni sekvence
    ifj.write(str);
}