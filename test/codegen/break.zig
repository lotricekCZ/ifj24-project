// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//0  // ifj2024 exit code
//0  // interpreter exit code
// --- EXPECTED OUTPUT ---
//Simple for break at: 2
//Nested for break at: 1-1-1
//Simple while break at: 2
//Nested while break at: 1-1-1
//While || break at: 2
//Nested while || break at: 1-1-1
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    // Test 1: Simple for
    ifj.write("Simple for break at: ");
    var i: i32 = 0;
    while (i < 5) {
        if (i == 2) {
            ifj.write(i);
            break;
        }
        i = i + 1;
    }
    ifj.write("\n");

    // Test 2: Triple nested for
    ifj.write("Nested for break at: ");
    var x1: i32 = 0;
    while (x1 < 3) {
        var x2: i32 = 0;
        while (x2 < 3) {
            var x3: i32 = 0;
            while (x3 < 3) {
                if (x1 == 1 and x2 == 1 and x3 == 1) {
                    ifj.write(x1);
                    ifj.write("-");
                    ifj.write(x2);
                    ifj.write("-");
                    ifj.write(x3);
                    break;
                }
                x3 = x3 + 1;
            }
            if (x1 == 1 and x2 == 1) {
                break;
            }
            x2 = x2 + 1;
        }
        if (x1 == 1) {
            break;
        }
        x1 = x1 + 1;
    }
    ifj.write("\n");

    // Test 3: Simple while
    ifj.write("Simple while break at: ");
    var y: i32 = 0;
    while (y < 5) {
        if (y == 2) {
            ifj.write(y);
            break;
        }
        y = y + 1;
    }
    ifj.write("\n");

    // Test 4: Triple nested while
    ifj.write("Nested while break at: ");
    var y1: i32 = 0;
    while (y1 < 3) {
        var y2: i32 = 0;
        while (y2 < 3) {
            var y3: i32 = 0;
            while (y3 < 3) {
                if (y1 == 1 and y2 == 1 and y3 == 1) {
                    ifj.write(y1);
                    ifj.write("-");
                    ifj.write(y2);
                    ifj.write("-");
                    ifj.write(y3);
                    break;
                }
                y3 = y3 + 1;
            }
            if (y1 == 1 and y2 == 1) {
                break;
            }
            y2 = y2 + 1;
        }
        if (y1 == 1) {
            break;
        }
        y1 = y1 + 1;
    }
    ifj.write("\n");

    // Test 5: Simple while ||
    ifj.write("While || break at: ");
    var z: i32 = 0;
    while (z < 5) {
        if (z == 2) {
            ifj.write(z);
            break;
        }
        z = z + 1;
    }
    ifj.write("\n");

    // Test 6: Triple nested while ||
    ifj.write("Nested while || break at: ");
    var z1: i32 = 0;
    while (z1 < 3) {
        var z2: i32 = 0;
        while (z2 < 3) {
            var z3: i32 = 0;
            while (z3 < 3) {
                if (z1 == 1 and z2 == 1 and z3 == 1) {
                    ifj.write(z1);
                    ifj.write("-");
                    ifj.write(z2);
                    ifj.write("-");
                    ifj.write(z3);
                    break;
                }
                z3 = z3 + 1;
            }
            if (z1 == 1 and z2 == 1) {
                break;
            }
            z2 = z2 + 1;
        }
        if (z1 == 1) {
            break;
        }
        z1 = z1 + 1;
    }
    ifj.write("\n");
}
