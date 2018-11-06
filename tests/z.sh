#!/bin/bash

while read line
do
    if [ -x ${line} ]; then
        echo "${line}"
    fi
done < usr_lib64 > usr_lib64_executables
