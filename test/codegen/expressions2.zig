// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//0
//0
// --- EXPECTED OUTPUT ---
//15
//45
//33
//0x1.9p3
//36
//60
//7
//9
//100
//42
//null
//84
//0x1.91eb851eb851fp1
//10
//0x1.48p4
//15
//null
//50
//200
//25
//8
//4
//null
//10
//84
//0
//15
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn returnNumber() i32 {
    return (((((((((42)))))))));
}

pub fn returnFloat() f64 {
    return 3.14;
}

pub fn main() void {
    // Test with integer variables and their modification
    var x: i32 = 5;
    var y: i32 = 10;
    x = x + y;
    ifj.write(x); // 15
    ifj.write("\n");
    y = y + 15;

    // Arithmetic operations with variables
    var a: i32 = 5;
    var b: i32 = 20;
    a = a * 5;
    ifj.write(a + b); // 45
    ifj.write("\n");
    b = b - 10;

    // Complex expressions with variables
    var val1: i32 = 3;
    var val2: i32 = 5;
    val1 = val1 + 5;
    ifj.write(val1 * val2 - 7); // 33
    ifj.write("\n");
    val2 = val2 + 10;

    // Working with decimal numbers
    var float1: f64 = 5.0;
    var float2: f64 = 7.5;
    float1 = float1 + 0.0;
    ifj.write(float1 + float2); // 0x1.9p3 -> tyto vyrazy jsou je pro hexa hodnoty
    ifj.write("\n");
    float2 = float2 + 10.0;

    // More complex arithmetic expressions with variables
    var mult1: i32 = 5;
    var mult2: i32 = 6;
    mult1 = mult1 + 1;
    ifj.write(mult1 * mult2); // 36
    ifj.write("\n");
    mult2 = mult2 * 10;

    // Multiplication and division with variables
    var num3: i32 = 20;
    var num4: i32 = 3;
    num3 = num3 + 0;
    ifj.write(num3 * num4); // 60
    ifj.write("\n");
    num4 = num4 * 2;

    // Expressions with typecast and variables
    var cast1: i32 = 3;
    var cast2: i32 = 4;
    cast1 = cast1 + 0;
    ifj.write(cast1 + cast2); // 7
    ifj.write("\n");
    cast1 = cast1 + 1;
    cast2 = cast2 - 1;

    // Combined arithmetic operations
    var math1: i32 = 4;
    var math2: i32 = 5;
    math1 = math1 + 0;
    ifj.write(math1 + math2); // 9
    ifj.write("\n");
    math2 = math2 - 1;

    // Working with constants and variables
    const const1: i32 = 40;
    var var1: i32 = 60;
    var1 = var1 + const1;
    ifj.write(var1); // 100
    ifj.write("\n");

    // Optional variables - i32
    var opt1: ?i32 = 42;
    ifj.write(opt1 orelse 0); // 42
    ifj.write("\n");
    opt1 = 25;

    var opt2: ?i32 = null;
    if (opt2) |value| {
        ifj.write(value);
    } else {
        ifj.write("null"); // null
    }
    ifj.write("\n");
    opt2 = 84;

    // Operations with optional values
    var opt3: ?i32 = 84;
    ifj.write(opt3 orelse 0); // 84
    ifj.write("\n");
    opt3 = 42;

    // Optional variables - f64
    var optFloat1: ?f64 = 3.14;
    ifj.write(optFloat1 orelse 0.0); // 0x1.91eb851eb851fp1
    ifj.write("\n");
    optFloat1 = 10.0;

    // Modification of optional variables
    var optNum1: ?i32 = 5;
    optNum1 = 10;
    ifj.write(optNum1 orelse 0); // 10
    ifj.write("\n");

    // Working with optional f64
    var optFloat2: ?f64 = 20.5;
    ifj.write(optFloat2 orelse 0.0); // 0x1.48p4
    ifj.write("\n");
    optFloat2 = 15.0;

    // Combination of constants and optional variables
    const constNum: i32 = 15;
    var optNum2: ?i32 = constNum;
    ifj.write(optNum2 orelse 0); // 15
    ifj.write("\n");
    optNum2 = 25;

    // Setting optional to null
    var optNum3: ?i32 = 42;
    optNum3 = null;
    if (optNum3) |value| {
        ifj.write(value);
    } else {
        ifj.write("null"); //null
    }
    ifj.write("\n");

    // Arithmetic operations with optional
    var optNum4: ?i32 = 25;
    var optNum5: ?i32 = 25;
    ifj.write((optNum4 orelse 0) + (optNum5 orelse 0)); // 50
    ifj.write("\n");
    optNum5 = 75;
    optNum4 = 100;

    // Combination of different types
    const constVal: i32 = 100;
    var optNum6: ?i32 = 100;
    ifj.write((optNum6 orelse 0) + constVal); // 200
    ifj.write("\n");
    optNum6 = 50;

    // Modification of optional values
    var optNum7: ?i32 = 200;
    optNum7 = 25;
    ifj.write(optNum7 orelse 0); // 25
    ifj.write("\n");

    // Multiplication with optional
    var optNum8: ?i32 = 2;
    optNum8 = 8;
    ifj.write(optNum8 orelse 0); // 8
    ifj.write("\n");

    // Complex expressions with optional
    var optNum9: ?i32 = 4;
    optNum9 = optNum9 orelse 0 * 4;
    ifj.write(optNum9 orelse 0 * 4); // 4
    ifj.write("\n");

    // Setting to null and check
    var optNum10: ?i32 = 100;
    optNum10 = null;
    if (optNum10) |value| {
        ifj.write(value);
    } else {
        ifj.write("null");
    }
    ifj.write("\n");

    // Combination of functions and optional
    var optNum11: ?i32 = 10;
    ifj.write(optNum11 orelse returnNumber()); // 10
    ifj.write("\n");
    optNum11 = 25;

    // Modification through function
    var optNum12: ?i32 = returnNumber() * 2;
    ifj.write(optNum12 orelse 0); // 84
    ifj.write("\n");
    optNum12 = 42;

    // Assignment from function
    var optNum13: ?i32 = null;
    ifj.write(optNum13 orelse 0); // 0
    ifj.write("\n");
    optNum13 = 15;

    // Final test with constant
    const finalConst: i32 = 15;
    var finalVar: ?i32 = finalConst;
    ifj.write(finalVar orelse 0); // 15
    ifj.write("\n");
    finalVar = 25;
}
