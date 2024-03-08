#!/bin/bash

usage="$(basename "$0") [FILE] [REPETITIONS] [MAXSIZE]
Concatenate the specified file a specified number of times so that the length
of the resulting file in bytes does not exceed the specified number.

positional arguments:
  FILE         File for concatenation
  REPEATITION  Number of file concatenations
  MAXSIZE      The maximum size of the resulting file

options:
  -h, --help   show this help message and exit"

if [ "$1" = "-h" ]; then
  echo "Usage: $usage"
  exit 0
fi



FILE=$1
REPETITIONS=$2
MAXSIZE=$3



if [ "$#" -lt 1 ]; then
  echo "USAGE, i.e. -h"
  exit 1
fi


if [ $(wc -c < "$FILE") -eq 0 ]; then
  exit 1
fi


size=$(wc -c < "$FILE")
in_file=$(cat "$FILE")

for (( i=1; i <= $REPETITIONS; i++ )); do
  nsize=$(echo -n "$in_file" | wc -c)
  if [ $(($nsize + $size)) -lt $MAXSIZE ]; then
    echo $in_file >> $FILE
    size=$(wc -c < "$FILE")
  else
    echo $in_file > $FILE
    exit 1
  fi
done
