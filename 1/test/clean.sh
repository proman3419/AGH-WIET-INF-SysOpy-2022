#!/bin/bash

if [ $(basename "$PWD") = "test" ]; then
    echo "rm ./*.txt"
    rm ./*.txt
fi
