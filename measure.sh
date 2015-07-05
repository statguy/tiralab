#!/bin/bash

rm -f btree.csv skiplist.csv

for ((degree=2; degree<41; degree+=1))
do
  echo Testing B-tree degree $degree, iteration $i...
  ./test btree $degree 1000 keys.txt 0 >> btree.csv
done

for ((level=1; level<41; level+=1))
do
  #for p in 0 0.3 0.5 0.7 1.0
  for p in 0.5
  do
    echo Testing Skiplist level $level, probability $p, iteration $i...
    ./test skiplist $level $p 1000 keys.txt 0 >> skiplist.csv
  done
done
