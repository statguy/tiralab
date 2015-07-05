#!/bin/bash

rm -f btreetest.txt skiplisttest.txt

for ((degree=2; degree<502; degree+=5))
do
  echo Testing B-tree degree $degree...
  ./test btree $degree 10 keys.txt 1 2>&1 >> btreetest.txt
done

for ((level=1; level<1002; level+=5))
do
  for p in 0 0.3 0.5 0.7 1.0
  do
    echo Testing Skiplist level $level, probability $p...
    ./test skiplist $level $p 10 keys.txt 1 2>&1 >> skiplisttest.txt
  done
done

cat btreetest.txt | grep VALIDATE
cat skiplisttest.txt | grep VALIDATE
