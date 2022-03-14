#!/bin/bash

if [ $(basename "$PWD") = "test" ]; then
    rm ./*.txt
fi
