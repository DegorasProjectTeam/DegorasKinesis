'''
    Copyright(C) Milethos Technologies SLU. 2025
    All rights reserved. Reproduction in whole or in
    part is prohibited without the written consent of
    the copyright holder.
'''

import os
import re
import argparse

def extract_initial_block(lines):
    block = []
    inside_block = False
    i = 0

    while i < len(lines):
        line = lines[i]
        stripped = line.lstrip()

        if stripped.startswith('/*'):
            inside_block = True

        if inside_block:
            block.append(line.rstrip())
            if '*/' in stripped:
                inside_block = False
        elif not stripped.strip():
            block.append(line.rstrip())
        else:
            break
        i += 1

    return block, lines[i:]


def clean_lines(lines):
    preserved_block, remaining = extract_initial_block(lines)

    cleaned_lines = preserved_block[:]
    empty_last = False

    for original_line in remaining:
        line = original_line.rstrip()
        stripped = line.lstrip()

        if stripped.startswith('//') or stripped.startswith('/*') or stripped.startswith('*/') or stripped.startswith('*'):
            continue

        line = re.sub(r'\s*///<.*$', '', line)
        line = re.sub(r'\s*//\!.*$', '', line)

        line = line.rstrip()

        if line.strip() == '':
            if empty_last:
                continue
            cleaned_lines.append('')
            empty_last = True
        else:
            cleaned_lines.append(line)
            empty_last = False

    return cleaned_lines


def clean_and_save_file(filepath, input_root, output_root):
    with open(filepath, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    cleaned = clean_lines(lines)

    relative_path = os.path.relpath(filepath, input_root)
    output_path = os.path.join(output_root, relative_path)
    os.makedirs(os.path.dirname(output_path), exist_ok=True)

    with open(output_path, 'w', encoding='utf-8') as f:
        f.write('\n'.join(cleaned) + '\n')

    print(f" Cleaned: {relative_path} -> {output_path}")


def process_recursive(input_dir, output_dir):
    for root, _, files in os.walk(input_dir):
        for file in files:
            if file.endswith(('.h', '.tpp')):
                full_path = os.path.join(root, file)
                clean_and_save_file(full_path, input_dir, output_dir)


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Clean C++ header files and remove inline/documentation comments.")
    parser.add_argument("--input", required=True, help="Input folder to scan recursively")
    parser.add_argument("--output", required=True, help="Output folder to store cleaned headers")
    args = parser.parse_args()

    if not os.path.isdir(args.input):
        print(f" Input path does not exist: {args.input}")
        exit(1)

    print(f"- Scanning: {args.input}")
    print(f"- Output to: {args.output}")

    process_recursive(args.input, args.output)

    print(" All header files cleaned and saved.")
