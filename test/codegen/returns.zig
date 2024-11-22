// --- INPUT ---
//
// --- EXPECTED EXIT CODES ---
//0  // ifj2024 exit code
//0  // interpreter exit code
// --- EXPECTED OUTPUT ---
//50005000
// --- TEST CODE ---
const ifj = @import("ifj24.zig");

pub fn main() void {
    if (1 == 1) {
        if (1 == 1) {
            ifj.write(add(50, 50, 2));
        } else {}

        if (1 == 1) {
            return;
        } else {
            return;
        }
        if (1 == 1) {} else {}
    } else {
        return;
    }
    ifj.write(add(50, 50, 2));
    ifj.write("\n");
}

pub fn add(a: i32, b: i32, c: i32) i32 {
    if (1 == 1) {
        if (1 == 1) {
            if (1 == 1) {
                if (1 == 1) {
                    if (1 == 1) {
                        if (1 == 1) {
                            if (1 == 1) {}
                        }
                    }
                }
            } else {
                if (1 == 1) {
                    if (1 == 1) {
                        if (1 == 1) {
                            if (1 == 1) {
                                if (1 == 1) {
                                    if (1 == 1) {}
                                }
                            }
                        }
                    }
                }
            }
        }
        if (1 == 0) {
            return 0;
        } else {
            return a * b * c;
        }
    } else {
        if (1 == 1) {
            if (1 == 1) {
                if (1 == 1) {
                    if (1 == 1) {
                        if (1 == 1) {
                            if (1 == 1) {}
                        }
                    }
                }
            } else {
                if (1 == 1) {
                    if (1 == 1) {
                        if (1 == 1) {
                            if (1 == 1) {
                                if (1 == 1) {
                                    if (1 == 1) {}
                                }
                            }
                        }
                    }
                }
            }
        }
        return 1;
    }
}
