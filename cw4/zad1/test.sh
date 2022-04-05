#!/bin/bash

test()
{
    test_all_actions output_fork
    test_all_actions output_exec
}

test_all_actions()
{
    test_sub $1 ignore
    test_sub $1 handler
    test_sub $1 mask
    test_sub $1 pending
}

test_sub()
{
    echo "##################################################"
    echo "Testing $1, the action is '$2'"
    echo "##################################################"
    ./$1 $2
    echo
}

test
