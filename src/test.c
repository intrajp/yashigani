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

int calc_hash ( char *path )
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256( (unsigned char *)path, strlen(path), hash );
    showHexString( hash, SHA256_DIGEST_LENGTH );

    return ( 0 );
}

/*
void calc_hash ( const char *path )
{
    int err = 0;
    char buff [ PATH_MAX ];
    memset ( buff, '\0', PATH_MAX ); 
    snprintf ( buff, PATH_MAX, "sha256sum %s", path );
    err = system ( buff );
    if ( err )
        fprintf(stderr, "command failed: %s (%d)\n", buff, err);    
    else
        printf("%s",buff);
}
*/

int main ( int argc, char *argv[] )
{
    //char *textvector = "The quick brown fox jumps over the lazy dog";
    //calc_hash ( "/bin/ab" );
    if (1) 
        puts("it is 1");
    else
        puts("it is not 1");
    
    return ( 0 );
}
