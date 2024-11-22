// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//0  // ifj2024 exit code
//0  // interpreter exit code
// --- EXPECTED OUTPUT ---
//Simple for continue: 0 2 3 4
//Nested for continue: 0-0 0-2 1-0 1-2 2-0 2-2
//Simple while continue: 0 2 3 4
//Nested while continue: 0-0 0-2 1-0 1-2 2-0 2-2
//While || continue: 0 2 3 4
//Nested while || continue: 0-0 0-2 1-0 1-2 2-0 2-2
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    // Test 1: Simple for
    ifj.write("Simple for continue: ");
    var i: i32 = 0;
    while (i < 5) {
        if (i == 1) {
            i = i + 1;
            continue;
        }
        ifj.write(i);
        if (i < 4) {
            ifj.write(" ");
        }
        i = i + 1;
    }
    ifj.write("\n");

    // Test 2: Double nested for with continue
    ifj.write("Nested for continue: ");
    var x1: i32 = 0;
    while (x1 < 3) {
        var x2: i32 = 0;
        while (x2 < 3) {
            if (x2 == 1) {
                x2 = x2 + 1;
                continue;
            }
            ifj.write(x1);
            ifj.write("-");
            ifj.write(x2);
            if (!(x1 == 2 and x2 == 2)) {
                ifj.write(" ");
            }
            x2 = x2 + 1;
        }
        x1 = x1 + 1;
    }
    ifj.write("\n");

    // Test 3: Simple while
    ifj.write("Simple while continue: ");
    var y: i32 = 0;
    while (y < 5) {
        if (y == 1) {
            y = y + 1;
            continue;
        }
        ifj.write(y);
        if (y < 4) {
            ifj.write(" ");
        }
        y = y + 1;
    }
    ifj.write("\n");

    // Test 4: Double nested while
    ifj.write("Nested while continue: ");
    var y1: i32 = 0;
    while (y1 < 3) {
        var y2: i32 = 0;
        while (y2 < 3) {
            if (y2 == 1) {
                y2 = y2 + 1;
                continue;
            }
            ifj.write(y1);
            ifj.write("-");
            ifj.write(y2);
            if (!(y1 == 2 and y2 == 2)) {
                ifj.write(" ");
            }
            y2 = y2 + 1;
        }
        y1 = y1 + 1;
    }
    ifj.write("\n");

    // Test 5: Simple while ||
    ifj.write("While || continue: ");
    var z: i32 = 0;
    while (z < 5) {
        if (z == 1) {
            z = z + 1;
            continue;
        }
        ifj.write(z);
        if (z < 4) {
            ifj.write(" ");
        }
        z = z + 1;
    }
    ifj.write("\n");

    // Test 6: Double nested while ||
    ifj.write("Nested while || continue: ");
    var z1: i32 = 0;
    while (z1 < 3) {
        var z2: i32 = 0;
        while (z2 < 3) {
            if (z2 == 1) {
                z2 = z2 + 1;
                continue;
            }
            ifj.write(z1);
            ifj.write("-");
            ifj.write(z2);
            if (!(z1 == 2 and z2 == 2)) {
                ifj.write(" ");
            }
            z2 = z2 + 1;
        }
        z1 = z1 + 1;
    }
    ifj.write("\n");
}
