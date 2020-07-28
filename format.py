import os
import sys

_type = ['c', 'h']

for root, dirs, files in os.walk("."):
    for file in files:
        ext_name = file.split("."[-1])
        if (ext_name[-1] in _type):
            os.system("clang-format -i " + root + "/" + file)
