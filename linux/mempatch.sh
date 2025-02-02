#!/bin/bash

for mem_range in $(cat /proc/$1/maps | grep rw | awk '{print $1}'); do

    mem_addr1=$(echo $mem_range | awk -F "-" '{print $1}')
    mem_addr2=$(echo $mem_range | awk -F "-" '{print $2}')

    for i in $(./finder $1 $mem_addr1 $mem_addr2 $2); do
        ./writer $1 $i $3
    done

done
