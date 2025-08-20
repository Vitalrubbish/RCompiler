#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log_info() {
    echo -e "${GREEN}[INFO] $1${NC}"
}
log_warn() {
    echo -e "${YELLOW}[WARN] $1${NC}"
}
log_error() {
    echo -e "${RED}[ERROR] $1${NC}"
}

mkdir -p test_results/semantic_official

log_info "开始官方语义检查测试（仅判断退出码）..."
total_tests=0
passed_tests=0

for in_file in testcases_official/Semantic/*.rx; do
    if [ -f "$in_file" ]; then
        filename=$(basename "$in_file")
        total_tests=$((total_tests + 1))

        log_info "测试用例: $filename"
        echo "----------------------------------------"
        echo "输入文件内容:"
        cat "$in_file"
        echo "----------------------------------------"
        echo "程序输出:"

        output=$(cmake-build-debug/RCompiler < "$in_file" 2>&1)
        exit_code=$?
        echo "$output"
        echo "----------------------------------------"

        echo "$output" > "test_results/semantic_official/${filename}_output.txt"

        if [ $exit_code -eq 0 ]; then
            log_info "✅ 测试通过: $filename (exit_code=0)"
            passed_tests=$((passed_tests + 1))
        else
            log_error "❌ 测试失败: $filename (exit_code=$exit_code)"
        fi
        echo "========================================"
    fi
done

log_info "测试完成!"
log_info "总计测试: $total_tests"
log_info "通过测试: $passed_tests"
log_info "失败测试: $((total_tests - passed_tests))"

echo "测试完成时间: $(date)" > test_results/semantic_official/summary_exitcode.txt
echo "总计测试: $total_tests" >> test_results/semantic_official/summary_exitcode.txt
echo "通过测试: $passed_tests" >> test_results/semantic_official/summary_exitcode.txt
echo "失败测试: $((total_tests - passed_tests))" >> test_results/semantic_official/summary_exitcode.txt

if [ $passed_tests -eq $total_tests ]; then
    log_info "🎉 所有测试通过!"
    exit 0
else
    log_error "💥 有测试用例失败"
    exit 1
fi