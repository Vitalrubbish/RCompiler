#!/bin/bash

# 设置颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# 打印带颜色的信息
log_info() {
    echo -e "${GREEN}[INFO] $1${NC}"
}

log_warn() {
    echo -e "${YELLOW}[WARN] $1${NC}"
}

log_error() {
    echo -e "${RED}[ERROR] $1${NC}"
}

# 创建测试结果目录
mkdir -p test_results/parser

# 运行Parser测试
log_info "开始Parser测试..."
total_tests=0
passed_tests=0

for test_file in testcases/Parser/*; do
    if [ -f "$test_file" ]; then
        filename=$(basename "$test_file")
        test_name="${filename%.*}"
        total_tests=$((total_tests + 1))

        log_info "测试用例: $filename"
        echo "----------------------------------------"
        echo "输入文件内容:"
        cat "$test_file"
        echo "----------------------------------------"
        echo "程序输出:"

        # 运行程序并捕获输出
        output=$(cmake-build-debug/RCompiler < "$test_file" 2>&1)
        exit_code=$?

        echo "$output"
        echo "----------------------------------------"

        # 保存输出到文件
        echo "$output" > "test_results/parser/${test_name}_output.txt"

        if [ $exit_code -eq 0 ]; then
            log_info "✅ 测试通过: $filename"
            passed_tests=$((passed_tests + 1))
        else
            log_error "❌ 测试失败: $filename (退出码: $exit_code)"
        fi
        echo "========================================"
    fi
done

# 打印测试统计
log_info "测试完成!"
log_info "总计测试: $total_tests"
log_info "通过测试: $passed_tests"
log_info "失败测试: $((total_tests - passed_tests))"

# 保存测试结果摘要
echo "测试完成时间: $(date)" > test_results/parser/summary.txt
echo "总计测试: $total_tests" >> test_results/parser/summary.txt
echo "通过测试: $passed_tests" >> test_results/parser/summary.txt
echo "失败测试: $((total_tests - passed_tests))" >> test_results/parser/summary.txt

if [ $passed_tests -eq $total_tests ]; then
    log_info "🎉 所有测试通过!"
    exit 0
else
    log_error "💥 有测试用例失败"
    exit 1
fi