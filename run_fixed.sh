#!/bin/bash
make all

program=rfs
eval server

# happy path tests - WRITE with first filename
eval $program WRITE data/file1.txt
echo "exit status = " $?

# happy path tests - STOP
eval $program STOP
echo "exit status = " $?