// Hello World example in IFJ24
// run it on Merlin.fit.vutbr.cz by: zig run hello.zig

const ifj = @import("ifj24.zig");

pub fn ha() i32 {
    return 3;
}

pub fn main() void {
  const y : i32 = ha();
  ifj.write("Hello from IFJ"); // one-parameter function only
  ifj.write(y);
  ifj.write("\n");
}
