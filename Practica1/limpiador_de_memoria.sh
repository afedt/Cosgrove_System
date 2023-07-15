#!/bin/bash
ipcs|awk '{print $2}' > llista.txt

for i in `cat llista.txt`
do
echo $i
ipcrm -m $i
done
for i in `cat llista.txt`
do
echo $i
ipcrm -s $i
done
for i in `cat llista.txt`
do
echo $i
ipcrm -q $i
done

