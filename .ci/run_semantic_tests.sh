#!/bin/bash

# 测试脚本用于语义分析阶段
# 忽略行末换行符差异进行比较

# 创建测试结果目录
mkdir -p test_results/semantic

# 日志函数
log_info() {
    echo "[INFO] $1"
}

log_warn() {
    echo "[WARN] $1"
}

log_error() {
    echo "[ERROR] $1"
}

# 初始化计数器
total_tests=0
passed_tests=0

log_info "开始语义分析测试..."
echo "测试开始时间: $(date)"

# 遍历测试用例
for in_file in testcases/Semantic/in*.rx; do
if [ -f "$in_file" ]; then
    test_id=$(echo "$in_file" | grep -o '[0-9]\+')
    filename="in${test_id}.txt"
    out_file="testcases/Semantic/out${test_id}.txt"

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

    echo "$output" > "test_results/semantic/out${test_id}_output.txt"

    if [ -f "$out_file" ]; then
        # 使用diff比较时忽略行末空白和空行的差异
        diff -u -b -B "$out_file" "test_results/semantic/out${test_id}_output.txt" > "test_results/semantic/out${test_id}_diff.txt"
        if [ $? -eq 0 ]; then
            log_info "✅ 测试通过: $filename"
            passed_tests=$((passed_tests + 1))
        else
            # 如果diff有差异，再尝试比较去除行末空白后的内容
            # 创建去除行末空白的临时文件
            temp1=$(mktemp)
            temp2=$(mktemp)

            # 去除行末空白并保存到临时文件
            sed 's/[[:space:]]*$//' "$out_file" > "$temp1"
            sed 's/[[:space:]]*$//' "test_results/semantic/out${test_id}_output.txt" > "$temp2"

            # 比较处理后的文件
            diff -u -b -B "$temp1" "$temp2" > /dev/null 2>&1

            if [ $? -eq 0 ]; then
                log_info "✅ 测试通过: $filename (忽略行末空白差异)"
                passed_tests=$((passed_tests + 1))
            else
                log_error "❌ 测试失败: $filename (输出与预期不一致)"
                log_warn "差异如下："
                cat "test_results/semantic/out${test_id}_diff.txt"
            fi

            # 清理临时文件
            rm -f "$temp1" "$temp2"
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

echo "测试完成时间: $(date)" > test_results/semantic/summary.txt
echo "总计测试: $total_tests" >> test_results/semantic/summary.txt
echo "通过测试: $passed_tests" >> test_results/semantic/summary.txt
echo "失败测试: $((total_tests - passed_tests))" >> test_results/semantic/summary.txt

if [ $passed_tests -eq $total_tests ]; then
    log_info "🎉 所有测试通过!"
    exit 0
else
    log_error "💥 有测试用例失败"
    exit 1
fi