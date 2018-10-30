#include <openssl/sha.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void showHexString( unsigned char *hex, size_t n )
{
    for ( size_t i = 0; i < n; i++ )
    {
        printf("%02x", hex[i] );
    }
    printf("\n");
}

//int calc_hash ( char *fd )
int calc_hash ( FILE *fp )
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256( (unsigned char *)fd, strlen(fd), hash );
    showHexString( hash, SHA256_DIGEST_LENGTH );

    return ( 0 );
}

int main ( int argc, char *argv[] )
{
    //char *textvector = "The quick brown fox jumps over the lazy dog";
    FILE *fp; 
    calc_hash ( textvector );
    return ( 0 );
}
