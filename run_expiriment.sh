#!/bin/bash
#rm -rf build/*.csv

######## ENTER PATH TO YOUR JAVA BINARY HERE
#java="java"

threadcounts="1 2 4"
trials=1
sec=1

#cd build
#opts="-server -d64 -Xms3G -Xmx3G -Xbootclasspath/p:'../lib/scala-library.jar:../lib/deuceAgent.jar'" # ti kanei ayth h grammh 
command="$./benchmark"

dt=`date`
echo $dt
echo $dt > run.log

## LOOP THROUGH ALL EXPERIMENTS TWICE; THE FIRST TIME COUNTING THEM, AND THE SECOND TIME PERFORMING THEM
for counting in 0 1
do
    i=0

    if [ "$counting" -eq "0" ]
    then 
        echo "Counting experiments..."
    else
        printf "\n"
        st=$SECONDS
    fi

    ## ALL EXPERIMENTS FOR NON-STM DATA STRUCTURES
    for keysratio in 10000_30_20_0  
    do
	# print current trial
        for threads in $threadcounts
        do

                
                keys=`echo $keysratio | cut -f1 -d"_"`
                ins=`echo $keysratio | cut -f2 -d"_"`
                del=`echo $keysratio | cut -f3 -d"_"`
                find=`echo $keysratio | cut -f4 -d"_"`
		i=`expr $i + 1`
		
                if [ "$counting" -eq "0" ]; then echo "$i"; continue; fi  
#den xreiazetai to param?

               args="$ins $del $find $trials $threads $keys $sec test$keys-$ins-$del-$threads.txt"
                echo -n "ex$i: "
                echo "$command $args"
		
                ./benchmark $args
    #            top -n 1 > top-trials$i.log


            
        done
    done

maxi=$i
done

dt=`date`
echo $dt

