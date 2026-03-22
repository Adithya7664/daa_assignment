#!bin/bash

g++ -03 -march=native daa.cpp -o bc

echo "Output of Wiki-Vote dataset"
./bc Wiki-Vote.txt < WikiVoteoutput.txt

echo "Output of Email-Enron dataset"
./bc Email-Enron.txt < EmailEnronoutput.txt