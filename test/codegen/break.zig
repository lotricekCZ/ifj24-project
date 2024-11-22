// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//0  // ifj2024 exit code
//0  // interpreter exit code
// --- EXPECTED OUTPUT ---
//Simple for break: 0 1 2
//Simple while break: 0 1 2
//While-opt break: 0 1 2
//Nested for outer: 0
//Nested for inner: 0 1
//Nested while outer: 0
//Nested while inner: 0 1
//Break with condition: 0 1 2 3 4
//Multiple breaks in while: First 0 1 Second 0 1
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    // Test 1: Simple for loop break
    ifj.write("Simple for break: ");
    var i: i32 = 0;
    for ("0123456789") |_| {
        if (i == 3) break;
        ifj.write(i);
        ifj.write(" ");
        i += 1;
    }
    ifj.write("\n");

    // Test 2: Simple while loop break
    ifj.write("Simple while break: ");
    i = 0;
    while (i < 10) {
        if (i == 3) break;
        ifj.write(i);
        ifj.write(" ");
        i += 1;
    }
    ifj.write("\n");

    // Test 3: While with optional break
    ifj.write("While-opt break: ");
    i = 0;
    while (i < 10) {
        i += 1;
        if (i == 3) break;
        ifj.write(i);
        ifj.write(" ");
    }
    ifj.write("\n");

    // Test 4: Nested for loops with break
    ifj.write("Nested for outer: ");
    for (ifj.string("0123")) |x| {
        ifj.write(x);
        ifj.write(" ");
        ifj.write("\nNested for inner: ");
        for (0..3) |y| {
            if (y == 2) break;
            ifj.write(y);
            ifj.write(" ");
        }
    }
    ifj.write("\n");

    // Test 5: Nested while loops with break
    ifj.write("Nested while outer: ");
    var x: i32 = 0;
    var y: i32 = 0;
    while (x < 3) : (x += 1) {
        ifj.write(x);
        ifj.write(" ");
        ifj.write("\nNested while inner: ");
        y = 0;
        while (y < 3) {
            y = y + 1;
            if (y == 2) break;
            ifj.write(y);
            ifj.write(" ");
        }
        break; // Break after first iteration
    }
    ifj.write("\n");

    // Test 6: Break with condition
    ifj.write("Break with condition: ");
    i = 0;
    while (i < 10) {
        i += 1;
        if (i > 4) break;
        ifj.write(i);
        ifj.write(" ");
    }
    ifj.write("\n");

    // Test 7: Multiple breaks in the same program
    ifj.write("Multiple breaks in while: ");
    i = 0;
    while (i < 5) {
        i += 1;
        if (i == 2) break;
        ifj.write("First ");
        ifj.write(i);
        ifj.write(" ");
    }
    i = 0;
    while (i < 5) {
        i += 1;
        if (i == 2) break;
        ifj.write("Second ");
        ifj.write(i);
        ifj.write(" ");
    }
    ifj.write("\n");
}
