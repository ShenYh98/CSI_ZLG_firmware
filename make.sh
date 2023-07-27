#!/bin/bash

# 检查脚本参数
if [ "$1" != "x86" ] && [ "$1" != "clean" ]; then
  echo "请输入一个参数，可选参数为 x86 或 clean"
  exit 1
fi

# 获取脚本所在目录
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# 进入build目录
cd "$script_dir/build"

# 根据输入的参数选择操作
if [ $1 = "clean" ]; then
  # 清除构建
  rm -rf *
  echo "构建已清除"
elif [ $1 = "x86" ]; then
  # 进行cmake构建
  if [ -n "$2" ]; then
    cmake -DLOG_LEVEL="$2" ..
  else
    cmake ..
  fi

  # 检查cmake是否成功
  if [ $? -eq 0 ]; then
    # 使用make编译程序
    make

    # 检查make编译是否成功
    if [ $? -eq 0 ]; then
      echo "编译成功"
    else
      echo "make编译失败"
      exit 1
    fi
  else
    echo "cmake构建失败"
    exit 1
  fi
else
  echo "参数错误，请输入 x86 或 clean"
  exit 1
fi