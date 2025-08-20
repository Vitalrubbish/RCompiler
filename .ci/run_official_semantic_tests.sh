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

log_info "开始官方语义检查测试（输出需与预期一致）..."
total_tests=0
passed_tests=0

for in_file in testcases_official/Semantic/*.rx; do
    if [ -f "$in_file" ]; then
        filename=$(basename "$in_file")
        test_id="${filename#in}"
        test_id="${test_id%.rx}"
        out_file="testcases_official/Semantic/out${test_id}.txt"
        total_tests=$((total_tests + 1))

        log_info "测试用例: $filename"
        echo "----------------------------------------"
        echo "输入文件内容:"
        cat "$in_file"
        echo "----------------------------------------"
        echo "程序输出:"

        output=$(cmake-build-debug/RCompiler < "$in_file" 2>&1)
        echo "$output"
        echo "----------------------------------------"

        echo "$output" > "test_results/semantic_official/out${test_id}_output.txt"

        if [ -f "$out_file" ]; then
            diff -u "$out_file" "test_results/semantic_official/out${test_id}_output.txt" > "test_results/semantic_official/out${test_id}_diff.txt"
            if [ $? -eq 0 ]; then
                log_info "✅ 测试通过: $filename"
                passed_tests=$((passed_tests + 1))
            else
                log_error "❌ 测试失败: $filename (输出与预期不一致)"
                log_warn "差异如下："
                cat "test_results/semantic_official/out${test_id}_diff.txt"
            fi
        else
            log_error "❌ 测试失败: $filename (缺少预期输出文件: out${test_id}.txt)"
        fi
        echo "========================================"
    fi
done

log_info "测试完成!"
log_info "总计测试: $total_tests"
log_info "通过测试: $passed_tests"
log_info "失败测试: $((total_tests - passed_tests))"

echo "测试完成时间: $(date)" > test_results/semantic_official/summary.txt
echo "总计测试: $total_tests" >> test_results/semantic_official/summary.txt
echo "通过测试: $passed_tests" >> test_results/semantic_official/summary.txt
echo "失败测试: $((total_tests - passed_tests))" >> test_results/semantic_official/summary.txt

if [ $passed_tests -eq $total_tests ]; then
    log_info "🎉 所有测试通过!"
    exit 0
else
    log_error "💥 有测试用例失败"
    exit 1
fi