#!/bin/bash

echo -e "\nTEST 1.1:"
./test btree 1 1 keys.txt 0
echo -e "\nTEST 1.2:"
./test btree 2 -1 keys.txt 0
echo -e "\nTEST 1.3:"
./test btree 2 1 keys.txt -1
echo -e "\nTEST 1.4:"
./test btree 2 1 duplicate.txt 0
echo -e "\nTEST 1.5:"
./test btree 2 1 invalid.txt 0
echo -e "\nTEST 1.6:"
./test btree 2 1 nonexistent 0

echo -e "\nTEST 2.1:"
./test skiplist 0 .5 1 keys.txt 0
echo -e "\nTEST 2.2:"
./test skiplist 1 -1.5 1 keys.txt 0
echo -e "\nTEST 2.3:"
./test skiplist 1 .5 -1 keys.txt 0
echo -e "\nTEST 2.4:"
./test skiplist 1 .5 1 keys.txt -1
echo -e "\nTEST 2.5:"
./test skiplist 1 .5 1 duplicate.txt 0
echo -e "\nTEST 2.6:"
./test skiplist 1 .5 1 invalid.txt 0
echo -e "\nTEST 2.7:"
./test skiplist 1 .5 1 lastkey.txt 0
echo -e "\nTEST 2.8:"
./test skiplist 1 .5 1 nonexistent 0
