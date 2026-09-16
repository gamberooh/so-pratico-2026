import os
import sys

def inotab(max_depth=None) :
    results = []
    base_path = os.getcwd()
    base_depth = base_path.count(os.path.sep)
    for root, dirs, files in os.walk(base_path):
        curr_depth = root.count(os.path.sep) - base_depth
        if max_depth is not None and curr_depth >= max_depth:
            dirs[:] = []
        for i_name in dirs + files:
            full_path = os.path.join(root,i_name)
            try:
                inode = os.stat(full_path).st_ino

                rel_path = os.path.relpath(full_path, base_path)
                results.append((inode, rel_path))
            

            except OSError:
                continue
    results.sort(key=lambda x: x[0])

    for inode, path in results:
        print(f"iNode: {inode}, relative path: {path}")


if __name__ == "__main__":
    if (len(sys.argv) == 1):
        inotab()    
    else:
        inotab(int(sys.argv[1]))
