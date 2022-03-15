#!/bin/bash

FILE_BASE_NAME=$1
FILE_SIZE=$2 # [MB]
FILES_COUNT=$3

for i in $(seq 0 $FILES_COUNT) 
do
    FILE_NAME="$FILE_BASE_NAME$i.txt"
    dd if=/dev/urandom bs=786438 count=$FILE_SIZE | base64 > $FILE_NAME
done
