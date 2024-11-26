#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'
CODE_FILE="prg.code"
INPUT_FILE="prg.in"
OUTPUT_FILE="prg.out"
TEMP_EXPECTED="expected.tmp"
TEMP_ACTUAL="actual.tmp"
TEMP_INPUTS="inputs.tmp"

SKIP_ZIG_CHECK=0
RUN_MAKE=0

# Zpracování parametrů
while getopts "xm" opt; do
    case $opt in
        x) SKIP_ZIG_CHECK=1 ;;
        m) RUN_MAKE=1 ;;
        \?) echo "Invalid option: -$OPTARG" >&2; exit 1 ;;
    esac
done
shift $((OPTIND-1))

XLOGIN="xhubacv00"
SERVER="$XLOGIN@merlin.fit.vutbr.cz"
REMOTE_TMP="/tmp/$USER_$(date +%s)"

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="$(cd "$SCRIPT_DIR/../../build" && pwd)"

# Spuštění make pokud je požadováno
if [ $RUN_MAKE -eq 1 ]; then
    make -j16 > /dev/null
    if [ $? -ne 0 ]; then
        echo -e "${RED}Make failed${NC}"
        exit 1
    fi
fi

check_file() {
    local zig_file="$1"
    [ "$(basename "$zig_file")" = "ifj24.zig" ] && return
    [ -f "$zig_file" ] || return

    # Kontrola očekávaných návratových kódů
    local expected_ifj=$(awk '/^\/\/ --- EXPECTED EXIT CODES ---/{flag=1;next} /^\/\/ ---/{flag=0} flag && /^\/\//{if(NR==FNR){match($0, /[0-9]+/);code=substr($0,RSTART,RLENGTH);exit}} END{print code}' "$zig_file")
    local expected_server=$(awk '/^\/\/ --- EXPECTED EXIT CODES ---/{flag=1;next} /^\/\/ ---/{flag=0} flag && /^\/\//{if(++n==2){match($0, /[0-9]+/);print substr($0,RSTART,RLENGTH);exit}}' "$zig_file")
    
    # Pokud je některý z očekávaných kódů nenulový, přeskočit kontrolu
    [ "$expected_ifj" != "0" ] || [ "$expected_server" != "0" ] && return

    zig_basename=$(basename "$zig_file")
    
    # Extrahování vstupů pro test
    awk '/^\/\/ --- INPUT ---/{flag=1;next} /^\/\/ ---/{flag=0} flag && /^\/\//{print substr($0,3)}' "$zig_file" > "$TEMP_INPUTS"
    
    if [ "$zig_basename" != "ifj24.zig" ]; then
        cat "$zig_file" | ssh $SERVER "cat > $REMOTE_TMP/$zig_basename"
        cat "$TEMP_INPUTS" | ssh $SERVER "cat > $REMOTE_TMP/test.in"
        if ! ssh $SERVER "cd $REMOTE_TMP && zig run $zig_basename < test.in" > /dev/null 2>&1; then
            echo -e "${RED}Error: File $zig_file is not compilable with zig run${NC}"
            echo "Compilation output:"
            ssh $SERVER "cd $REMOTE_TMP && zig run $zig_basename < test.in" 2>&1
            ssh $SERVER "rm -rf $REMOTE_TMP"
            exit 1
        fi
    fi
}

ssh $SERVER "mkdir -p $REMOTE_TMP"
cat "$SCRIPT_DIR/ifj24.zig" | ssh $SERVER "cat > $REMOTE_TMP/ifj24.zig"

# Kontrola spustitelnosti pouze pokud není -x
if [ $SKIP_ZIG_CHECK -eq 0 ]; then
    echo "Checking if test files are compilable with zig..."
    if [ $# -eq 0 ]; then
        for zig_file in "$SCRIPT_DIR"/*.zig; do
            check_file "$zig_file"
        done
    else
        for test_name in "$@"; do
            test_file="$SCRIPT_DIR/${test_name}.zig"
            if [ -f "$test_file" ]; then
                check_file "$test_file"
            else
                echo -e "${RED}Test file not found: ${test_name}${NC}"
                exit 1
            fi
        done
    fi
    echo -e "${GREEN}All test files are compilable${NC}"
    echo "----------------------------------------"
fi

TOTAL_TESTS=0
PASSED_TESTS=0

run_test() {
    local zig_file="$1"
    [ "$(basename "$zig_file")" = "ifj24.zig" ] && return
    [ -f "$zig_file" ] || return
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    basename=$(basename "$zig_file" .zig)
    echo "Testing: $basename"

    awk '/^\/\/ --- INPUT ---/{flag=1;next} /^\/\/ ---/{flag=0} flag && /^\/\//{print substr($0,3)}' "$zig_file" > "$TEMP_INPUTS"
    expected_ifj=$(awk '/^\/\/ --- EXPECTED EXIT CODES ---/{flag=1;next} /^\/\/ ---/{flag=0} flag && /^\/\//{if(NR==FNR){match($0, /[0-9]+/);code=substr($0,RSTART,RLENGTH);exit}} END{print code}' "$zig_file")
    expected_server=$(awk '/^\/\/ --- EXPECTED EXIT CODES ---/{flag=1;next} /^\/\/ ---/{flag=0} flag && /^\/\//{if(++n==2){match($0, /[0-9]+/);print substr($0,RSTART,RLENGTH);exit}}' "$zig_file")
    awk '/^\/\/ --- EXPECTED OUTPUT ---/{f=1;next} /^\/\/ ---/{if(f){exit}} f{if(/^\/\//){print substr($0,3)}else if(NF==0){print ""}}' "$zig_file" > "$TEMP_EXPECTED"
    
    expected_ifj=${expected_ifj:-0}
    expected_server=${expected_server:-0}
    
    cat "$zig_file" | "$BUILD_DIR/ifj2024" > "$OUTPUT_FILE" 2>/dev/null
    ifj_exit_code=$?
    
    if [ $ifj_exit_code -eq 0 ]; then
        cp "$OUTPUT_FILE" "$CODE_FILE"
        cat "$CODE_FILE" | ssh $SERVER "cat > $REMOTE_TMP/$CODE_FILE"
        
        if [ -s "$TEMP_INPUTS" ]; then
            server_output=$(cat "$TEMP_INPUTS" | ssh $SERVER "/pub/courses/ifj/ic24int/linux/ic24int $REMOTE_TMP/$CODE_FILE" 2>&1)
        else
            server_output=$(ssh $SERVER "/pub/courses/ifj/ic24int/linux/ic24int $REMOTE_TMP/$CODE_FILE" 2>&1)
        fi
        server_exit_code=$?
        printf "%s\n" "$server_output" > "$TEMP_ACTUAL"
    else
        server_output=""
        server_exit_code=0
        > "$TEMP_ACTUAL"
    fi
    
    output_matches=true
    if ! cmp -s "$TEMP_EXPECTED" "$TEMP_ACTUAL"; then
        output_matches=false
    fi
    
    if [ "$ifj_exit_code" -eq "$expected_ifj" ] && 
       [ "$server_exit_code" -eq "$expected_server" ] && 
       $output_matches; then
        echo -e "${GREEN}✓ Test passed${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}✗ Test failed${NC}"
        if [ "$ifj_exit_code" -ne "$expected_ifj" ] || [ "$server_exit_code" -ne "$expected_server" ]; then
            echo "Expected IFJ exit code: $expected_ifj, got: $ifj_exit_code"
            echo "Expected server exit code: $expected_server, got: $server_exit_code"
        fi
        if ! $output_matches; then
            echo "Output diff (< expected, > actual):"
            diff --color "$TEMP_EXPECTED" "$TEMP_ACTUAL" || true
        fi
    fi
    
    echo "----------------------------------------"
}

cd "$SCRIPT_DIR"

if [ $# -eq 0 ]; then
    for zig_file in *.zig; do
        run_test "$zig_file"
    done
else
    for test_name in "$@"; do
        test_file="${test_name}.zig"
        if [ -f "$test_file" ]; then
            run_test "$test_file"
        else
            echo -e "${RED}Test file not found: ${test_name}${NC}"
        fi
    done
fi

ssh $SERVER "rm -rf $REMOTE_TMP"
rm -f "$TEMP_EXPECTED" "$TEMP_ACTUAL" "$TEMP_INPUTS" "$CODE_FILE" "$OUTPUT_FILE"

if [ $PASSED_TESTS -eq $TOTAL_TESTS ]; then
    echo -e "${GREEN}All tests passed: $PASSED_TESTS/$TOTAL_TESTS${NC}"
else
    echo -e "${RED}Tests passed: $PASSED_TESTS/$TOTAL_TESTS${NC}"
fi
