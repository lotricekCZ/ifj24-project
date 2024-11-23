const ifj = @import("ifj24.zig");

// Hlavni funkce
pub fn main() void {
    // var nullable_a: ?i32 = null;
    // var nullable_b: ?i32 = 5;
    // var result1: ?i32 = nullable_a orelse nullable_b; // result1 = 5
    // var result2: i32 = (nullable_a orelse 10).?; // result2 = 10
    // var result3: i32 = nullable_b.?; // result3 = 5

    // // Pouzitie null vo vyrazoch
    // var compare: bool = nullable_a == nullable_b; // compare = false
    // var unreachable_result: i32 = (nullable_a orelse unreachable).?; // panic: reached unreachable code

    //----------------------------------------------------------------------------

    // var a: ?i32 = null;
    // var b: i32 = 5;
    // var complex_orelse: i32 = ((a orelse b) * 2) + 1; // complex_orelse = 11

    //----------------------------------------------------------------------------

    var int_x: i32 = 10;
    var float_y: f64 = 3.5;

    // Kombinacia celeho a desatinneho cisla
    var mixed_op: f64 = ifj.i2f(int_x) + float_y * 2.0; // mixed_op = 17.0

    // // Negativne hodnoty
    // var negative_op: f64 = -float_y * ifj.i2f(int_x); // negative_op = -35.0

    // Nedefinovane celociselne delenie (delenie nulou)
    var div_by_zero: i32 = int_x; // 0; // chybny pripad - runtime chyba

    //----------------------------------------------------------------------------

    var empty_str: []u8 = ifj.string("");
    var valid_str: []u8 = ifj.string("Hello World!");
    var concat_result: []u8 = ifj.concat(empty_str, valid_str); // concat_result = "Hello World!"

    // Podretazec s nespravnymi indexmi
    var substr_valid: ?[]u8 = ifj.substring(valid_str, 0, 5); // substr_valid = "Hello"
    // var substr_invalid1: ?[]u8 = ifj.substring(valid_str, -5, 5); // substr_invalid1 = null
    var substr_invalid2: ?[]u8 = ifj.substring(valid_str, 5, 50); // substr_invalid2 = null

    //----------------------------------------------------------------------------

    var bool_a: bool = true;
    var bool_b: bool = false;

    // Kombinacie logickych operatorov
    var logic_mix1: bool = bool_a and (bool_b or not bool_a); // logic_mix1 = false
    var logic_mix2: bool = (bool_a or bool_b) and not bool_b; // logic_mix2 = true

    // Porovnania s null a hodnotami
    var nullable_int: ?i32 = null;
    var compare_null: bool = nullable_int == null and bool_a; // compare_null = true

    //----------------------------------------------------------------------------

    var complex_logic: bool = ((a orelse b) > 2) and (not (b == 0)); // complex_logic = true

    //----------------------------------------------------------------------------

    var large_num: i32 = 2147483647; // Max hodnota pre i32
    var small_num: i32 = -2147483648; // Min hodnota pre i32
    var overflow_test: i32 = large_num + 1; // runtime chyba

    //----------------------------------------------------------------------------

    const str: []u8 = ifj.string("IFJ24");

    for (str) |i| {
        const znak: i32 = @as(i32, i);
        ifj.write(znak); 
        ifj.write(" ");
    }

    //----------------------------------------------------------------------------

    const text: []u8 = ifj.string("Priklad textu.");

    for (text) |j| {
        const znak: i32 = @as(i32, j);
        if (znak >= 65 and znak <= 90) { 
            ifj.write(znak);
        }
    }

    //----------------------------------------------------------------------------

    const str: []u8 = ifj.string("");

    for (str) |i| {
        const znak: i32 = @as(i32, i);
        ifj.write(znak);
        ifj.write(" ");
    }

    //----------------------------------------------------------------------------

    const str: []u8 = ifj.string("A");

    for (str) |i| {
        const znak: i32 = @as(i32, i);
        ifj.write(znak);
        ifj.write("\n");
    }

    //----------------------------------------------------------------------------

    const str: []u8 = ifj.string("Aa1! @");

    for (str) |i| {
        const znak: i32 = @as(i32, i);
        ifj.write(znak);
        ifj.write(" ");
    }

    //----------------------------------------------------------------------------

    const str: []u8 = ifj.repeat("A", 1000000);

    for (str) |i| {
        const znak: i32 = @as(i32, i);
        ifj.write(znak);
        ifj.write(" ");
        break if (i == 10);
    }

    //----------------------------------------------------------------------------

    const str: []u8 = ifj.string("Test");

    for (str) |i| {
        const znak: i32 = @as(i32, i);
        if (znak > 128 or znak < 0) { 
            ifj.write("Error\n");
        } else {
            ifj.write(znak);
            ifj.write(" ");
        }
    }

    //----------------------------------------------------------------------------

    const str: []u8 = ifj.string("A B");

    for (str) |i| {
        const znak: i32 = @as(i32, i);
        ifj.write(znak);
        ifj.write(" ");
    }

    //----------------------------------------------------------------------------

    const str: []u8 = ifj.string("123abcXYZ");

    for (str) |i| {
        const znak: i32 = @as(i32, i);
        if (znak >= 48 and znak <= 57) { 
            ifj.write("Digit ");
        } else if (znak >= 65 and znak <= 90) { 
            ifj.write("Uppercase ");
        } else if (znak >= 97 and znak <= 122) { 
            ifj.write("Lowercase ");
        }
    }

    //----------------------------------------------------------------------------

    const str: []u8 = ifj.string("IFJ24 Language");

    var count: i32 = 0;
    for (str) |i| {
        const znak: i32 = @as(i32, i);
        if (znak >= 65 and znak <= 90) { 
            count = count + 1;
        }
    }
    ifj.write("Count: ");
    ifj.write(count);

    //----------------------------------------------------------------------------

    const str: []u8 = ifj.string("FindXHere");

    for (str) |i| {
        const znak: i32 = @as(i32, i);
        if (znak == @as(i32, "X")) {
            ifj.write("Found X\n");
            break;
        }
    }

    //----------------------------------------------------------------------------

    //----------------------------------------------------------------------------

    //----------------------------------------------------------------------------

    //----------------------------------------------------------------------------

    var a: i32 = 5;
    var b: i32 = 10;
    var c: f64 = 2.5;
    var d: ?i32 = null;

    var a: i32 = 10;
    var b: i32 = 5;
    var c: f64 = 2.5;

    var vysledok: bool = (a + b) * 2 > (c / 2.0) - 1;

    var a: i32 = 10;
    var b: i32 = 5;
    var c: bool = true;

    var vysledok: bool = ((a > b) and (c) or (a == b));

    var a: i32 = 10;
    var b: i32 = 5;

    var vysledok: i32 = if (a > b) then a else b;

    var a: i32 = 10;
    var b: i32 = 5;

    var vysledok: i32 = ifj.f2i(ifj.i2f(a) + ifj.i2f(b) / 2.0);

    var sum1: i32 = a + b;
    var sum2: f64 = b + c;
    var diff: i32 = b - a;
    var product: f64 = a * c;
    var quotient1: f64 = b / c;
    var quotient2: i32 = b; // a;

    var e: i32 = d orelse 15;
    var f: ?i32 = d orelse null;

    var g: bool = a == b;
    var h: bool = a != b;
    var i: bool = b > a;
    var j: bool = c <= 2.0;

    var k: bool = true;
    var l: bool = false;
    var n: bool = k and l;
    var o: bool = k or l;

    ifj.write("sum1 = ");
    ifj.write(sum1);
    ifj.write("\n");
    ifj.write("sum2 = ");
    ifj.write(sum2);
    ifj.write("\n");

    var x: i32 = 10;
    var y: f64 = 3.14;
    var z: bool = true;

    var vysledok: bool = (x * 2) > (y + 5.0) and z or not(x == 10);

    var a: ?i32 = null;
    var b: i32 = 5;

    var c: i32 = (a orelse b * 2).?;

    var str1: []u8 = ifj.string("Ahoj ");
    var str2: []u8 = ifj.string("svet!");

    var result: i32 = ifj.length(ifj.concat(str1, str2));

    var a: i32 = 10;
    var b: f64 = 3.14;

    var c: f64 = a + b * 2.0 - 1; // c = 15.28

    var d: i32 = -5

    var a: ?i32 = null;
    var b: i32 = 10;
    var c: ?i32 = 10;

    var d: bool = a == null; // d = true
    var e: bool = b == null; // e = false
    var f: bool = c == null; // f = false

    var g: bool = a == c;

    var str1: []u8 = ifj.string("Ahoj\n\"Sve'te \\\x22");
    var str2: []u8 = ifj.string("");

    var len1: i32 = ifj.length(str1); // len1 = 19
    var len2: i32 = ifj.length(str2); // len2 = 0

    var str3: []u8 = ifj.concat(str1, str2); // str3 = "Ahoj\n\"Sve'te \\\x22"

    var substr1: ?[]u8 = ifj.substring(str1, 0, len1); // substr1 = "Ahoj\n\"Sve'te \\\x22"
    var substr2: ?[]u8 = ifj.substring(str1, -1, 0); // substr2 = null (chyba - i < 0)
    var substr3: ?[]u8 = ifj.substring(str1, len1, len1 + 1); // substr3 = null (chyba - j > ifj.length(s))

    var d: i32 = a orelse b; // d = 10
    var e: ?f64 = a orelse c; // e = 2.5 (typ s null)
    var f: i32 = c orelse b; // f = 2.5

    var a: ?i32 = null;

    var b: i32 = a orelse unreachable;
    var c: i32 = (a orelse 10).?;
    //----------------------------------
