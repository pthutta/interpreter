#!/bin/bash

#simple script checks for differencies between input file and ast prettyprint of the interpreter
#./check.sh input_file_path int_binary_path

input=$1
binary=$2

$binary $input 2> output
diff -w $input output
rm output