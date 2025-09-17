#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

log_info() {
    echo -e "${GREEN}[INFO] $1${NC}"
}
log_error() {
    echo -e "${RED}[ERROR] $1${NC}"
}

total=0
passed=0

if [[ -n "$1" ]]; then
    case_dirs=("testcases_official/semantic-2/$1/")
else
    case_dirs=(testcases_official/semantic-2/*/)
fi

for case_dir in "${case_dirs[@]}"; do
    rx_file=$(find "$case_dir" -maxdepth 1 -name "*.rx" | head -n 1)
    json_file="${case_dir}testcase_info.json"
    if [[ -f "$rx_file" && -f "$json_file" ]]; then
        total=$((total+1))
        expected=$(jq '.compileexitcode' "$json_file")
        output_file=$(mktemp)
        # 仍然将 stdout 和 stderr 都重定向，以捕获所有输出
        cmake-build-debug/RCompiler < "$rx_file" > "$output_file" 2>&1
        compiler_exit_code=$?

        # 从输出文件中只读取最后一行作为判断依据
        output=$(tail -n 1 "$output_file" | tr -d '\r\n')
        rm "$output_file"

        # 首先检查编译器本身是否崩溃
        if [[ $compiler_exit_code -ne 0 ]]; then
            log_error "❌ 测试失败 (编译器崩溃): $case_dir"
            echo "编译器进程退出码: $compiler_exit_code"
            continue
        fi

        # 然后根据最后一行输出进行判断
        if [[ "$expected" == "0" ]]; then
            if [[ "$output" == "0" ]]; then
                log_info "✅ 测试通过: $case_dir"
                passed=$((passed+1))
            else
                log_error "❌ 测试失败: $case_dir"
                echo "预期输出: 0"
                echo "实际输出: $output"
            fi
        elif [[ "$expected" == "-1" ]]; then
            # 这里我们假设只要输出不是0，就认为是正确的错误报告
            if [[ "$output" != "0" ]]; then
                log_info "✅ 测试通过: $case_dir"
                passed=$((passed+1))
            else
                log_error "❌ 测试失败: $case_dir"
                echo "预期输出: 非0的错误信息"
                echo "实际输出: $output"
            fi
        else
            log_error "❌ 测试失败: $case_dir"
            echo "未知的 compileexitcode: $expected"
        fi
    fi
done

echo "总计测试: $total"
echo "通过测试: $passed"
echo "失败测试: $((total-passed))"