#!/bin/bash

echo "Compiling"
g++ -O3 -march=native main.cpp -o bc

echo "Running wiki-Vote"
./bc wiki-Vote.txt > output_wiki.txt

echo "Running email-Enron"
./bc email-Enron.txt > output_enron.txt

echo "Done!"
