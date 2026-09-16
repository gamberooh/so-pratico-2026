import os
import sys

def same_size(f, d):
    try:
        st = os.stat(os.path.abspath(f))
    except:
        print("error in stat f")
        return

    print("same sized\n")
    for root, dirs, files in os.walk(d):
        for filename in files:
            filepath = os.path.join(root, filename)
            try:
                curr_st = os.stat(filepath)
                if st.st_size == curr_st.st_size:
                    if st.st_ino != curr_st.st_ino or st.st_dev != curr_st.st_dev:
                        print(f"{filepath}\n")
            except OSError:
                continue

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Error {sys.argv[0]}\n", file=sys.stderr)
        sys.exit(1)
    else:
        same_size(sys.argv[1], sys.argv[2])
