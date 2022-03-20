#!/bin/bash

SAMPLE_FILE_PATH="$1"
DEST_FILE_PATH="$2"
TIMES=$3

SAMPLE=$(cat "$SAMPLE_FILE_PATH")
echo "$SAMPLE" > $DEST_FILE_PATH
for i in $(seq 1 $TIMES)
do
    echo "$SAMPLE" >> $DEST_FILE_PATH
done
