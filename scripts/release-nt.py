#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
release to module nt
"""

import os
import re
import zipfile
import shutil

# 项目根目录
ROOT_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))

# 版本声明正则表达式
# 只匹配文件开头的版权声明，不匹配函数注释
VERSION_HEADER_PATTERN = re.compile(r'\A\s*(///.*(?:\r?\n))+')

# 需要处理的文件扩展名
SOURCE_EXTENSIONS = ['.cpp', '.h', '.hpp', '.c', ".cpp0", ".0cpp"]

# 需要转换编码的目录
ENCODE_DIRS = ['examples', 'test']

# 需要删除的文件
FILES_TO_DELETE = [
    os.path.join(ROOT_DIR, 'README_zh.md'),
    os.path.join(ROOT_DIR, 'README.md'),
    os.path.join(ROOT_DIR, 'doxyfile'),
    os.path.join(ROOT_DIR, '.gitmodule')
]

# 压缩包名称
ZIP_NAME = 'ast-nt.zip'

# 排除的目录
EXCLUDE_DIRS = ['data', 'docs', '.git', 'build', '.xmake', ".trae", ".vscode", ".github"]

def remove_version_header(file_path):
    """删除文件开头的连续注释块（以 /// 开头）以及中间的空行"""
    # 用 utf-8-sig 读取，自动跳过可能存在的 BOM
    with open(file_path, 'r', encoding='utf-8-sig') as f:
        lines = f.readlines()

    # 找到第一个既不是 /// 注释也不是空行的行的索引
    start_idx = 0
    for i, line in enumerate(lines):
        stripped = line.lstrip()  # 去除行首空白，便于判断
        if stripped.startswith('///') or stripped == '' or stripped.isspace():
            # 注释行或空行，继续跳过
            continue
        else:
            # 遇到有效代码行，从此处开始保留
            start_idx = i
            break
    else:
        # 如果整个文件都是注释/空行，则保留原文件不变（防止清空文件）
        print(f"All lines are comments/empty, skipping: {file_path}")
        return

    # 如果有被跳过的行，则写回文件（使用无 BOM 的 UTF-8 编码）
    if start_idx > 0:
        new_lines = lines[start_idx:]
        with open(file_path, 'w', encoding='utf-8') as f:
            f.writelines(new_lines)
        print(f"Removed version header from: {file_path}")

def convert_to_utf8_bom(file_path):
    """将文件转换为utf8-bom编码"""
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    
    with open(file_path, 'w', encoding='utf-8-sig') as f:
        f.write(content)
    print(f"Converted to utf8-bom: {file_path}")

def process_files():
    """处理所有文件"""
    for root, dirs, files in os.walk(ROOT_DIR):
        # 跳过不需要处理的目录
        dirs[:] = [d for d in dirs if d not in EXCLUDE_DIRS]
        
        for file in files:
            file_path = os.path.join(root, file)
            ext = os.path.splitext(file)[1]
            
            # 删除版本声明
            if ext in SOURCE_EXTENSIONS:
                remove_version_header(file_path)
            
            # 转换编码
            if any(encode_dir in root for encode_dir in ENCODE_DIRS) and ext in SOURCE_EXTENSIONS:
                convert_to_utf8_bom(file_path)

def delete_files():
    """删除指定文件"""
    for file_path in FILES_TO_DELETE:
        if os.path.exists(file_path):
            os.remove(file_path)
            print(f"Deleted file: {file_path}")

def create_zip():
    """创建压缩包"""
    zip_path = os.path.join(ROOT_DIR, ZIP_NAME)
    
    with zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
        for root, dirs, files in os.walk(ROOT_DIR):
            # 跳过不需要压缩的目录
            dirs[:] = [d for d in dirs if d not in EXCLUDE_DIRS]
            
            for file in files:
                file_path = os.path.join(root, file)
                # 计算相对路径
                arcname = os.path.relpath(file_path, ROOT_DIR)
                # 跳过压缩包本身
                if arcname == ZIP_NAME:
                    continue
                zipf.write(file_path, arcname)
                print(f"Added to zip: {arcname}")
    
    print(f"Created zip file: {zip_path}")

def main():
    """主函数"""
    print("Starting release process...")
    
    # 处理文件
    process_files()
    
    # 删除指定文件
    delete_files()
    
    # 创建压缩包
    create_zip()
    
    # 复原所有被修改的文件
    import subprocess
    print("Restoring modified files...")
    subprocess.run(['git', 'checkout', '--', '.'], cwd=ROOT_DIR, check=True)
    print("Files restored successfully!")
    
    print("Release process completed!")

if __name__ == '__main__':
    main()