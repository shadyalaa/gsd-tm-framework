#!/bin/sh
# the configuration files to running memcslap may be found here:
# http://search.cpan.org/~dmaki/Memcached-libmemcached-0.4202/src/libmemcached/docs/memslap.pod#Configuration_file
# example:
# key
# 64 64 1
# value
# 1024 1024 1
# cmd
# 0 0.1
# 1 0.9


DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

function write_config {
# $1 is the get operation fraction
# $2 is the key size
# $3 is the value size
    rm tmp-config
    calculation="1.0 - $1"
    write_fraction=`echo $calculation | bc -l`
    echo -e "key\n$2 $2 1\nvalue\n$3 $3 1\ncmd\n0 $write_fraction\n1 $1" > tmp-config
}

wait_until_finish() {
    pid3=$1
    echo "process is $pid3"
    LIMIT=120
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

readFraction[1]="1.0"
readFraction[2]="0.99"

export LD_LIBRARY_PATH="$DIR/../../backends/greentm/gcc-abi:$DIR/code:$HOME/lib/"
cd $DIR;
bash build-memcached-gcc.sh $1 $4 $5 1 $6
cd code;
for rf in {1..1}
do
    for division in 1 #10000
    do
    for concurrency in 256
    do
    for keySize in 64
    do
    for valueSize in 1024
    do
        rm ../../../auto-results/tmp
        write_config ${readFraction[$rf]} $keySize $valueSize
        echo "${readFraction[$rf]} | division $division | concurrency $concurrency | key $keySize | value $valueSize  | backend $1 | htm_retries $4 | threads $2 | attempt $3 | retry_policy $5"
        ./memcached -p 20000 -t $2 >> ../../../auto-results/$1-r${readFraction[$rf]}-d$division-c$concurrency-k$keySize-v$valueSize-retries$4-policy$5-th$2-att$3.data &
        pid_server=$!
        ./memcslap -F ../memslap-r.cnf --time=10s -s 127.0.0.1:20000 -c 1 -T 1 -B
        ./memcslap -F tmp-config --time=5s --division=$division -s 127.0.0.1:20000 -c $concurrency -T 16 -B >> ../../../auto-results/tmp &
        pid_client=$!
        wait_until_finish $pid_client;
        wait $pid_client;
        kill $pid_server
        wait_until_finish $pid_server;
        wait $pid_server
        cat ../../../auto-results/tmp | grep -v 'set success' >> ../../../auto-results/$1-r${readFraction[$rf]}-d$division-c$concurrency-k$keySize-v$valueSize-retries$4-policy$5-th$2-att$3.data
    done
    done
    done
    done
done
