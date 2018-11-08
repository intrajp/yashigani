##
##  utils.sh - helper tool for get_hash_for_list.c 
##  This file contains the contents of yashigani.
##
##  Copyright (C) 2018 Shintaro Fujiwara
##
##  This library is free software; you can redistribute it and/or
##  modify it under the terms of the GNU Lesser General Public
##  License as published by the Free Software Foundation; either
##  version 2.1 of the License, or (at your option) any later version.
##
##  This library is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
##  Lesser General Public License for more details.
##
##  You should have received a copy of the GNU Lesser General Public
##  License along with this library; if not, write to the Free Software
##  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
##  02110-1301 USA
##

#!/bin/bash

INPUT_FILE="${1}"
OUTPUT_FILE="calc_hash_pre.txt"
COM="calc_hash (\""

function usage()
{
    echo ""
    echo "${0} <inputfile>"
    echo ""
    echo "example:"
    echo "${0} usr_sbin.txt"
    echo ""
    exit 1
}

if [ -z "${INPUT_FILE}" ]; then

    echo "no input file !"
    usage
    exit 1
fi

function read_file_and_echo_com ()
{
    while read line
    do
        echo "${COM}${line}\");"
    done < "${INPUT_FILE}" > "${OUTPUT_FILE}"
}

read_file_and_echo_com
