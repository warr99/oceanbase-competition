#!/bin/bash

# 检查输入参数是否足够
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <input_log_file> <output_sorted_file>"
    exit 1
fi

input_log="$1"
output_sorted_file="$2"

# 删除包含类似[work_job_xxxx]的行
grep -v '\[work_job_[0-9]\{1,4\}\]' "$input_log" > ./temp_cleaned_log.log

# 提取带有 "cost=" 的行到一个临时文件
grep "cost=" "./temp_cleaned_log.log" > ./temp_cost_lines.log

# 提取 cost 值，并排序
awk -F'cost=' '{print $2, $0}' temp_cost_lines.log | sort -n -r | cut -d' ' -f2- > "$output_sorted_file"

# 清理临时文件
rm temp_cleaned_log.log temp_cost_lines.log

