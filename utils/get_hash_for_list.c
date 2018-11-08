/*
 *  get_hash_for_list - getting hash value for whit-list 
 *  This file contains the contents of yashigani.
 *
 *  Copyright (C) 2018 Shintaro Fujiwara
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA
 */

/* how to compile this file: gcc -o get_hash_for_list get_hash_for_list.c -lcrypto */

#include <openssl/sha.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void showHexString( const char *path, unsigned char *hex, size_t n )
{
    printf ( "%s  ",path );
    for ( size_t i = 0; i < n; i++ )
    {
        printf("%02x", hex[i] );
    }
    printf("\n");
}

int calc_hash ( char *path )
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256( (unsigned char *)path, strlen(path), hash );
    showHexString( path, hash, SHA256_DIGEST_LENGTH );

    return ( 0 );
}

int main ( int argc, char *argv[] )
{

    calc_hash ("/usr/lib64/BugpointPasses.so");

    return ( 0 );
}
