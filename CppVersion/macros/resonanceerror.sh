#!/bin/sh

echo "processing new file"
for i in {1..1000} 
do
if (( $i % 100 == 0 ))
then
echo "processing scan $i"
fi
./ResonanceFinder 1 0 ouprova2.root 1000 0 0.005 >> channeloutput0.txt
done
echo "processing new file"
for i in {1..1000}
do
if (( $i % 100 == 0 ))
then
echo "processing scan $i"
fi
./ResonanceFinder 1 0 ouprova2.root 1000 30 0.005 >> channeloutput30.txt
done
echo "processing new file"
for i in {1..1000}
do
if (( $i % 100 == 0 ))
then
echo "processing scan $i"
fi
./ResonanceFinder 1 0 ouprova2.root 1000 50 0.005 >> channeloutput50.txt
done
echo "processing new file"
for i in {1..1000}
do
if (( $i % 100 == 0 ))
then
echo "processing scan $i"
fi
./ResonanceFinder 1 0 ouprova2.root 1000 80 0.005 >> channeloutput80.txt
done
echo "processing new file"
for i in {1..1000}
do
if (( $i % 100 == 0 ))
then
echo "processing scan $i"
fi
./ResonanceFinder 1 0 ouprova2.root 1000 105 0.005 >> channeloutput105.txt
done
echo "processing new file"
for i in {1..1000}
do
if (( $i % 100 == 0 ))
then
echo "processing scan $i"
fi
./ResonanceFinder 1 0 ouprova2.root 1000 300 0.005 >> channeloutput300.txt
done


