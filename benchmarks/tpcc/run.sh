#!/bin/sh


benchmarks[1]="-s 96 -d 1 -o 1 -p 1 -r 1"
benchmarks[2]="-s 1 -d 96 -o 1 -p 1 -r 1"
benchmarks[3]="-s 1 -d 1 -o 96 -p 1 -r 1"
benchmarks[4]="-s 1 -d 1 -o 1 -p 96 -r 1"
benchmarks[5]="-s 1 -d 1 -o 1 -p 1 -r 96"
benchmarks[6]="-s 20 -d 20 -o 20 -p 20 -r 20"
benchmarks[7]="-s 4 -d 4 -o 4 -p 43 -r 45"

bStr[1]="s96-d1-o1-p1-r1"
bStr[2]="s1-d96-o1-p1-r1"
bStr[3]="s1-d1-o96-p1-r1"
bStr[4]="s1-d1-o1-p96-r1"
bStr[5]="s1-d1-o1-p1-r96"
bStr[6]="s20-d20-o20-p20-r20"
bStr[7]="s4-d4-o4-p43-r45"

benchmarks[10]="-m 32 -c 50 -w 1 -s 0 -d 0 -o 100 -p 0 -r 0 -c 50 -w 1 -s 4 -d 4 -o 4 -p 43 -r 45 -c 50 -w 32 -s 96 -d 1 -o 1 -p 1 -r 1"

bStr[10]="adapt"

wait_until_finish() {
    pid3=$1
    echo "process is $pid3"
    LIMIT=180
    for ((j = 0; j < $LIMIT; ++j)); do
        kill -s 0 $pid3
        rc=$?
        if [[ $rc != 0 ]] ; then
            echo "returning"
            return;
        fi
        sleep 1s
    done
    kill $pid3
}

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

export LD_LIBRARY_PATH=/home/ndiegues/boost_1_57_0/stage/lib/:/home/ndiegues/gsd-tm-framework/backends/greentm/gcc-abi/;
cd $DIR;
bash build-tpcc.sh $1 $4 $5 0 $6

for b in {10..10}
do
    cd $DIR;
    cd code;
    echo "${bStr[$b]} | backend $1 | htm_retries $4 | threads $2 | attempt $3 | retry_policy $5"
    ./tpcc -t 150 ${benchmarks[$b]}  -n $2 > ../../../auto-results/$1-${bStr[$b]}-retries$4-policy$5-th$2-att$3.data &
    pid3=$!; wait_until_finish $pid3; wait $pid3; rc=$?
    if [ $rc -eq 13 ] ; then
        echo "Error within: backend $1 | ${bStr[$b]} | htm_retries $4 | retry_policy $5 | threads $2 | attempt $3" >> ../../../auto-results/slow.out
    elif [[ $rc != 0 ]] ; then
        echo "Error within: backend $1 | ${bStr[$b]} | htm_retries $4 | retry_policy $5 | threads $2 | attempt $3" >> ../../../auto-results/error.out
    fi
done
