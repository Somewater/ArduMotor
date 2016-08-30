#!/usr/bin/env python

# cmakegen.py
# (c) Aleksander Alekseev 2015
# http://eax.me/

import sys
import os
import re
from os.path import expanduser
import glob

include_dirs = set()
source_files = set()

def recursive_search(path):
    # print(path)
    for file in os.listdir(path):
        if (file == ".") or (file == ".."):
            continue

        full_name = os.path.join(path, file)
        if os.path.isdir(full_name) and not os.path.islink(path):
            recursive_search(full_name)

        if re.search("\\.(c|cpp|cxx)$", file) is not None:
            source_files.add(full_name)
        elif re.search("\\.(h|hpp)$", file) is not None:
            include_dirs.add(path)

if len(sys.argv) < 2:
    print("Usage " + sys.argv[0] + " <path>")
    sys.exit(1)

start_path = sys.argv[1]
recursive_search(start_path)

if os.environ.get('ARDUINO_SDK_PATH'):
    recursive_search(os.environ['ARDUINO_SDK_PATH'])
else:
    sys.stderr.write('Specify ARDUINO_SDK_PATH\n')

arduino_libraries = expanduser("~") + '/Arduino/libraries'
if os.path.isdir(arduino_libraries):
    recursive_search(arduino_libraries)

boards_cores = glob.glob(expanduser("~") + "/.arduino*/packages/esp8266/hardware/esp8266/*/cores/esp8266/")
boards_libraries = glob.glob(expanduser("~") + "/.arduino*/packages/esp8266/hardware/esp8266/*/libraries/")
for boards_core in boards_cores:
    recursive_search(boards_core)
for library in boards_libraries:
    recursive_search(library)

print("cmake_minimum_required(VERSION 2.8)")
print("project(dummy_project)")

skip_chars = len(start_path) + 1

for dir in sorted(include_dirs):
    if dir[:skip_chars] == start_path + '/':
        print("include_directories(" + dir[skip_chars:] + ")")
    else:
        print("include_directories(" + dir + ")")

print("add_executable(dummy_executable")
for file in sorted(source_files):
    if file[:skip_chars] == start_path + '/':
        print("  " + file[skip_chars:])
    else:
        print("  " + file)
print(")")
