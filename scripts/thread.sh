#!/bin/bash

while read opt
do
  echo "$1 $opt" 
  $1 $opt &
  while (( $(jobs | wc -l) >= 4 )); do
    sleep 0.1
    jobs > /dev/null
  done
done
wait
