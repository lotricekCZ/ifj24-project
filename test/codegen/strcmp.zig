// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//0
//0
// --- EXPECTED OUTPUT ---
//0-110001
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = ifj.strcmp(ifj.string("a"), ifj.string("a"));
    ifj.write(a);

    const b = ifj.strcmp(ifj.string("a"), ifj.string("b"));
    ifj.write(b);

    const c = ifj.strcmp(ifj.string("b"), ifj.string("a"));
    ifj.write(c);

    const d = ifj.strcmp(ifj.string("a"), ifj.string("aa"));
    ifj.write(d);

    const e = ifj.strcmp(ifj.string("aa"), ifj.string("a"));
    ifj.write(e);

    const f = ifj.strcmp(ifj.string("aa"), ifj.string("aa"));
    ifj.write(f);

    const g = ifj.strcmp(ifj.string("Hello"), ifj.string(", World"));
    ifj.write(g);
}
