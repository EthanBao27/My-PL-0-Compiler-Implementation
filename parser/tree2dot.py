import sys

def count_indent(line):
    return len(line) - len(line.lstrip(' '))

def main():
    if len(sys.argv) != 3:
        print("用法: python tree2dot.py tree.txt output.dot")
        return

    infile = sys.argv[1]
    outfile = sys.argv[2]

    with open(infile) as f:
        lines = [line.rstrip() for line in f if line.strip()]

    dot = []
    dot.append("digraph ParseTree {")
    dot.append("  node [shape=box, style=filled, fillcolor=lightgray];")

    stack = []  # (indent, node_id)
    node_id = 0

    for line in lines:
        indent = count_indent(line)
        label = line.strip()
        current_id = f"n{node_id}"
        dot.append(f'  {current_id} [label="{label}"];')

        # 找到 parent
        while stack and stack[-1][0] >= indent:
            stack.pop()
        if stack:
            parent_id = stack[-1][1]
            dot.append(f'  {parent_id} -> {current_id};')

        stack.append((indent, current_id))
        node_id += 1

    dot.append("}")

    with open(outfile, 'w') as f:
        f.write('\n'.join(dot))

    print(f"已生成 DOT 文件: {outfile}")

if __name__ == "__main__":
    main()