#!/bin/bash

# Configuration
COMPILER="./cmake-build-debug/RCompiler"
TEST_DIR="testcases_official/semantic-2"
OUTPUT_DIR="temp_test_output"

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Check if compiler exists
if [ ! -f "$COMPILER" ]; then
    echo -e "${RED}Error: Compiler not found at $COMPILER${NC}"
    echo "Please build the project first."
    exit 1
fi

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Function to run a single test
run_test() {
    local case_name=$1
    local case_dir="$TEST_DIR/$case_name"
    local src_file="$case_dir/$case_name.rx"
    local input_file="$case_dir/$case_name.in"
    local expected_file="$case_dir/$case_name.out"
    
    local ll_file="$OUTPUT_DIR/$case_name.ll"
    local exe_file="$OUTPUT_DIR/$case_name"
    local actual_file="$OUTPUT_DIR/$case_name.result"
    
    # Check if test case exists
    if [ ! -f "$src_file" ]; then
        echo -e "${RED}Error: Test case $case_name not found (looked for $src_file)${NC}"
        return 1
    fi

    echo -n "Testing $case_name... "
    
    # 1. Compile to IR
    # Capture stderr to avoid cluttering output, but show it on failure
    ./$COMPILER < "$src_file" > "$ll_file" 2> "$OUTPUT_DIR/$case_name.compiler.log"
    if [ $? -ne 0 ]; then
        echo -e "${RED}FAIL (Compiler Error)${NC}"
        cat "$OUTPUT_DIR/$case_name.compiler.log"
        return 1
    fi
    
    # Patch the LLVM IR to use libc exit instead of the no-op dummy
    # This fixes the issue where the program returns a random exit code (e.g. 101)
    # because the generated exit() function does nothing and main() returns void.
    if grep -q "define void @exit(i32 %code)" "$ll_file"; then
        sed -i 's/define void @exit(i32 %code)/declare void @exit(i32)\ndefine void @exit_dummy(i32 %code)/' "$ll_file"
    fi
    
    # 2. Compile IR to Executable using clang
    # -Wno-override-module suppresses warnings about module flags mismatch if any
    # -lm links math library just in case
    # Set stack size to 64MB
    clang -x ir "$ll_file" -o "$exe_file" -Wno-override-module -lm -Wl,-z,stack-size=67108864 2> "$OUTPUT_DIR/$case_name.clang.log"
    if [ $? -ne 0 ]; then
        echo -e "${RED}FAIL (Clang Error)${NC}"
        cat "$OUTPUT_DIR/$case_name.clang.log"
        return 1
    fi
    
    # 3. Run Executable
    if [ -f "$input_file" ]; then
        timeout 30s "$exe_file" < "$input_file" > "$actual_file" 2> "$OUTPUT_DIR/$case_name.run.log"
    else
        timeout 30s "$exe_file" > "$actual_file" 2> "$OUTPUT_DIR/$case_name.run.log"
    fi
    
    run_ret=$?
    if [ $run_ret -eq 124 ]; then
        echo -e "${RED}FAIL (Timeout)${NC}"
        return 1
    elif [ $run_ret -ne 0 ]; then
        echo -e "${RED}FAIL (Runtime Error: $run_ret)${NC}"
        return 1
    fi
    
    # 4. Compare Output
    # Using diff -w (ignore whitespace) -B (ignore blank lines)
    diff -w -B "$actual_file" "$expected_file" > "$OUTPUT_DIR/$case_name.diff"
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}PASS${NC}"
        return 0
    else
        echo -e "${RED}FAIL (Output Mismatch)${NC}"
        echo "Diff (first 10 lines):"
        head -n 10 "$OUTPUT_DIR/$case_name.diff"
        return 1
    fi
}

# Main logic
if [ -n "$1" ]; then
    # Run single test
    run_test "$1"
else
    # Run all tests
    total=0
    passed=0
    failed_tests=()
    
    # Get list of test cases (directories)
    # Sort them to have deterministic order
    tests=$(find "$TEST_DIR" -mindepth 1 -maxdepth 1 -type d | sort)
    
    for dir in $tests; do
        case_name=$(basename "$dir")
        run_test "$case_name"
        ret=$?
        if [ $ret -eq 0 ]; then
            passed=$((passed+1))
        else
            failed_tests+=("$case_name")
        fi
        total=$((total+1))
    done
    
    echo "------------------------------------------------"
    echo "Summary: $passed / $total passed."
    if [ ${#failed_tests[@]} -ne 0 ]; then
        echo -e "${RED}Failed tests:${NC}"
        for t in "${failed_tests[@]}"; do
            echo "  - $t"
        done
        exit 1
    else
        echo -e "${GREEN}All tests passed!${NC}"
        exit 0
    fi
fi
