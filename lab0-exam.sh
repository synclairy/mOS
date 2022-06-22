#!/bin/bash
make
touch hello_os
mkdir hello_os_dir
cp hello_os.o hello_os_dir/hello_os.o
cp hello_os.o hello_os_dir/hello_os
make clean
sed 's/os_hello/?os_hello/g' hello_os.c > temp.txt
awk -F? '$0 ~ /os_hello/ {print $1}' temp.txt > hello_os.txt
rm temp.txt
