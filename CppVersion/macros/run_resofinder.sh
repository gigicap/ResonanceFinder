#!/bin/bash

declare -a arrayRT=("1" "100" "1000")
declare -a arrayNB=("0" "50" "80" "105" "150" "200" "300")
declare -a arrayWD=("0.05" "0.02" "0.01" "0.005" "0.003")

for i in "${arrayRT[@]}"
do
for j in "${arrayNB[@]}"
do
for s in "${arrayWD[@]}"
do 
./ResonanceFinder 1 1 out_rt${i}_nb${j}_wd${s}.root $i $j $s
echo "./ResonanceFinder 1 1 out _rt $i _nb $j _wd $s.root $i $j $s"
done 
done
done

exit 0
