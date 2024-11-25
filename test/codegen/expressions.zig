// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//0
//0
// --- EXPECTED OUTPUT ---
//2
//3.140000
//true
//false
//11
//1
//42
//true
//false
//true
//7
//2.500000
//false
//true
//84
//true
//false
//5
//true
//false
//10
//true
//15
//false
//true
//8
//3
//true
//false
//20
//15
//25
//true
//false
//8
//12.500000
//true
//30
//false
//true
//60
//true
//7
//9
//false
//true
//100
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn returnNumber() i32 {
    return 42;
}

pub fn returnFloat() f64 {
    return 3.14;
}

pub fn returnTrue() bool {
    return true;
}

pub fn returnFalse() bool {
    return false;
}

pub fn main() void {
    // Basic arithmetic operations
    ifj.write(1 + 1);
    ifj.write("\n");

    ifj.write(returnFloat());
    ifj.write("\n");

    // Logical operations
    ifj.write(true and true);
    ifj.write("\n");

    ifj.write(false or false);
    ifj.write("\n");

    // Complex arithmetic expressions
    ifj.write(5 + 2 * 3);
    ifj.write("\n");

    const q: i32 = 5;
    ifj.write(5 / 5);
    ifj.write("\n");

    // Function calls
    ifj.write(returnNumber());
    ifj.write("\n");

    // Logical operators
    ifj.write(returnTrue() and true);
    ifj.write("\n");

    ifj.write(!returnTrue());
    ifj.write("\n");

    ifj.write(5 > 3);
    ifj.write("\n");

    // Parentheses and operator priority
    ifj.write((3 + 2) * (5 - 4));
    ifj.write("\n");

    // Working with decimal numbers
    ifj.write(5.0 / 2.0);
    ifj.write("\n");

    // Comparison operators
    ifj.write(5 <= 3);
    ifj.write("\n");

    ifj.write(10 >= 5);
    ifj.write("\n");

    // Complex expressions
    ifj.write((returnNumber() * 2));
    ifj.write("\n");

    ifj.write(returnTrue() or returnFalse());
    ifj.write("\n");

    ifj.write(null == null);
    ifj.write("\n");

    // Combined expressions
    const z: i32 = 2;
    ifj.write(@as(i32, z) + 3);
    ifj.write("\n");

    ifj.write((5 > 3) and (10 < 20));
    ifj.write("\n");

    ifj.write((true and false) or (false and true));
    ifj.write("\n");

    // Conditional expressions
    ifj.write(if (true) 10 else 20);
    ifj.write("\n");

    // Complex logical expressions
    ifj.write(!(false or false) and true);
    ifj.write("\n");

    // Arithmetic operations with functions
    ifj.write(returnNumber() - 27);
    ifj.write("\n");

    // Comparison with null
    ifj.write(null == null);
    ifj.write("\n");

    ifj.write(true or false);
    ifj.write("\n");

    // Complex expressions with multiple operators
    ifj.write(2 * (5 - 1));
    ifj.write("\n");

    // Integer division
    ifj.write(9 / 3);
    ifj.write("\n");

    // Complex logical expressions
    ifj.write((5 > 3) and (10 != 5) and !false);
    ifj.write("\n");

    // Comparing function results
    ifj.write(returnNumber() == 43);
    ifj.write("\n");

    // Arithmetic operations with typecast
    const u: i32 = 4;
    ifj.write(@as(i32, u) * 5);
    ifj.write("\n");

    // NEW TESTS WITH VARIABLES:

    // Test with integer variables
    var x: i32 = 5;
    var y: i32 = 10;
    ifj.write(x + y); // 15
    ifj.write("\n");
    x = 15;
    y = 20;

    // Arithmetic operations with variables
    var a: i32 = 5;
    var b: i32 = 20;
    ifj.write(a + b); // 25
    ifj.write("\n");
    a = 10;
    b = 20;

    // Logical operations with variables
    var bool1: bool = true;
    var bool2: bool = false;
    ifj.write(bool1 and !bool2); // true
    ifj.write("\n");
    bool1 = false;
    bool2 = true;

    // Comparing variables
    var num1: i32 = 10;
    var num2: i32 = 20;
    ifj.write(num1 > num2); // false
    ifj.write("\n");
    num1 = 15;
    num2 = 20;

    // Complex expressions with variables
    var val1: i32 = 3;
    var val2: i32 = 5;
    ifj.write(val1 * val2 - 7); // 8
    ifj.write("\n");
    val1 = 5;
    val2 = 10;

    // Working with decimal variables
    var float1: f64 = 5.0;
    var float2: f64 = 7.5;
    ifj.write(float1 + float2); // 12.5
    ifj.write("\n");
    float1 = 10.0;
    float2 = 20.0;

    // Combining variables and constants in logical expressions
    var isGreater: bool = true;
    var isLess: bool = false;
    ifj.write(isGreater and !isLess); // true
    ifj.write("\n");
    isGreater = false;
    isLess = true;

    // More complex arithmetic expressions with variables
    var mult1: i32 = 5;
    var mult2: i32 = 6;
    ifj.write(mult1 * mult2); // 30
    ifj.write("\n");
    mult1 = 10;
    mult2 = 20;

    // Comparison with null for variables
    var nullVal: ?i32 = null;
    ifj.write(nullVal == null); // false
    ifj.write("\n");
    nullVal = 5;

    // Complex conditions with variables
    var cond1: bool = true;
    var cond2: bool = true;
    ifj.write(cond1 and cond2); // true
    ifj.write("\n");
    cond1 = false;
    cond2 = true;

    // Multiplication and division with variables
    var num3: i32 = 20;
    var num4: i32 = 3;
    ifj.write(num3 * num4); // 60
    ifj.write("\n");
    num3 = 10;
    num4 = 20;

    // Logical operations with multiple variables
    var logic1: bool = true;
    var logic2: bool = false;
    var logic3: bool = true;
    ifj.write(logic1 and !logic2 and logic3); // true
    ifj.write("\n");
    logic1 = false;
    logic2 = true;
    logic3 = false;

    // Expressions with typecast and variables
    var cast1: i32 = 3;
    var cast2: i32 = 4;
    ifj.write(cast1 + cast2); // 7
    ifj.write("\n");
    cast1 = 5;
    cast2 = 6;

    // Combined arithmetic operations
    var math1: i32 = 4;
    var math2: i32 = 5;
    ifj.write(math1 + math2); // 9
    ifj.write("\n");
    math1 = 10;
    math2 = 20;

    // Comparing different values
    var comp1: i32 = 10;
    var comp2: i32 = 20;
    ifj.write(comp1 >= comp2); // false
    ifj.write("\n");
    comp1 = 15;
    comp2 = 20;

    // Combining variables with functions
    var func1: bool = returnTrue();
    ifj.write(func1); // true
    ifj.write("\n");
    func1 = false;

    // Complex expressions with multiple variables
    var complex1: i32 = 50;
    var complex2: i32 = 50;
    ifj.write(complex1 + complex2); // 100
    ifj.write("\n");
    complex1 = 10;
    complex2 = 20;
}
