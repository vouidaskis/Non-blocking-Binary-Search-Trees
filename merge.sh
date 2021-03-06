#!/bin/sh

if [ "$#" -eq "0" ]; then
	echo "USAGE: $0 CSV_DATA [...]"
	exit 1
fi

cat $1 | head -1 > data.csv

i=0
for x in $@
do
	nlinesone=`cat $x | wc -l`
	nlines=`expr $nlinesone - 1`
	#echo "x = $x, nlinesone = $nlinesone, nlines = $nlines"
	text=`cat $x | tail -$nlines`
	echo "$text" >> data.csv
	i=`expr $i + 1`
	echo "done $i/$#"
done

