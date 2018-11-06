/*
 *  common.c - common definition 
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

#include <ctype.h> /* for is_space */
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <time.h> 
#include <unistd.h> 
#include <openssl/sha.h>
#include <openssl/rand.h>
#include "common.h"

char procfd_path_echo [ PATH_MAX ];
char buff [ PATH_MAX ];

int check_each_hash_and_path ( char **line, const char *path, const char *hash )
{
    const char s [ 4 ] = "  \t"; /* this is the delimiter */
    char *token = NULL;
    int break_id = 1;

    /* get the first token */
    token = strtok ( *line, s );
    /* reached EOF, so return 1 */
    if ( strcmp ( token, "EOF" ) == 0 )
    {
        break_id = 1;
        printf("path:%s\n",path);
        /* printf("--break_id--:%d\n",break_id); */
        return ( break_id );
    }
    /* did not match, but try again */
    if ( strcmp ( token, path ) != 0 )
    {
        /*
	 * printf("tokn:%s\n",token);
         * printf("path:%s\n",path);
         * puts("Not match !!");
	 */
        break_id = 3;
        /* printf("--break_id--:%d\n",break_id); */
        return ( break_id );
    }
    /* matched, so check next token ( hash ) */
    /*
     * else
     * {
     *  printf("tokn:%s\n",token);
     *  printf("path:%s\n",path);
     *  puts("path Match !!");
     * }
    */

    /* walk throuth other tokens */
    while ( token != NULL )
    {
        /* check hash */
        token = strtok ( NULL, s );
        if ( token == NULL )
            break;
        /* puts("--Next token--");
         * printf("tokn:%s\n",token);
	 */
        /* reached EOF, so return 1 */
        if ( strcmp ( token, "EOF" ) == 0 )
        {
            printf("path:%s\n",path);
            printf("hash:%s\n",hash);
            break_id  = 1;
            token = NULL;
            break;
        }
        /* Both path and hash matched, so return ( 0 ) */
        else if ( strcmp ( token, hash ) == 0 )
        {
            /*
	     * printf("tokn:%s\n",token);
             * printf("hash:%s\n",hash);
             * puts("hash ( and path ) Match !!");
	     */
            break_id  = 0;
            token = NULL;
            break;
        }
        /* did not match, could be security bleach, so return 2 */
        else
        {
            printf("tokn:%s\n",token);
            printf("hash:%s\n",hash);
            puts("Not match !!");
            break_id  = 2;
            token = NULL;
            break;
        } 
    }
    /* printf("--break_id--:%d\n",break_id); */
    return ( break_id );
}

int search_path_and_hash ( const char *path, const char *hash, const char *searchfile )
{
    FILE *fp;
    int lnr = 0;
    struct stat;
    char linebuf [ PATH_MAX ];
    char *line = "";
    int i;
    int break_id = -1;
    int ret = -1;

    /* open file */
    if ( ( fp = fopen ( searchfile, "r" ) ) == NULL )
    {
        printf("can't open file (%s): %s\n",searchfile,strerror(errno));
        exit ( EXIT_FAILURE );
    }

    /* read file and parse lines */
    while ( fgets ( linebuf, sizeof ( linebuf ), fp ) != NULL )
    {
        lnr++;
        line = linebuf;
        i = ( int ) strlen ( line );
        /* ignore comment lines */
        if ( ( line [ 0 ] == '#' ) || ( line [ 0 ] == ' ') )
            continue;
        /* strip newline */
        if ( ( i <= 0 ) || ( line [ i - 1 ] != '\n' ) )
        {
            printf("%s:%d: line too long or last line missing newline\n",searchfile,lnr);
            exit ( EXIT_FAILURE );
        }
        /* this for compare rightly */
        line [ i - 1 ] = '\0';
        /* for debug */
        /* printf("line:%s\n",line); */
        /* printf("path:%s\n",path); */
        /* printf("hash:%s\n",hash); */
        /* Failed to find */
        break_id = ( check_each_hash_and_path ( &line, path, hash ) );
        if ( break_id == 1 )
        {
            /* puts ( "check_each_hash_and_path returned 1"); */
            ret = -1;
            break;
        }
        /* Matched */
        else if ( break_id == 0 )
        {
            /* puts ( "check_each_hash_and_path returned 0"); */
            ret = 0;
            break;
        }
        /* !! security bleach !! */
        else if ( break_id == 2 )
        {
            /* puts ( "check_each_hash_and_path returned 2"); */
            ret = 2;
            break;
        }
        /* unused */
        else if ( break_id == 3 )
            ret = 3;
        else
            continue;
        /* strip trailing spaces */
        for ( i--; ( i > 0 ) && isspace ( line [ i -1 ] ) ; i-- )
            line [ i -1 ] = '\0';
    }
    /* after reading all lines, close the file pointer */
    fclose ( fp );

    return ( ret );
}

int check_executable ( int fd, struct stat *buf )
{
    if ( fstat ( fd, buf ) != 0 )
    {
        perror ( "fstat() error" );   
        return ( -1 );
    }
    else
        if ( buf->st_mode & S_IXUSR )
	    return ( 1 );

    return ( 0 );
}

const char *show_hex_string ( unsigned char *hex, size_t n )
{
    memset ( buff, '\0', PATH_MAX );
    char buff2 [ n + 1 ];
    memset ( buff2, '\0', sizeof ( n + 1 ) );
    for ( size_t i = 0; i < n; i++ )
    {
        snprintf ( buff2, n, "%02x", hex [ i ] );
        strncat ( buff, buff2, PATH_MAX -1 );
    }
    return buff;
}

const char *calc_hash ( const char *path )
{
    unsigned char hash [ SHA256_DIGEST_LENGTH ];
    SHA256 ( ( unsigned char * ) path, strlen ( path ), hash );
    show_hex_string ( hash, SHA256_DIGEST_LENGTH );

    return buff;
}

const char *get_path_name ( int fd )
{
    ssize_t path_len;
    char procfd_path [ PATH_MAX ];

    snprintf ( procfd_path, PATH_MAX,
               "/proc/self/fd/%d", fd );
    path_len = readlink ( procfd_path, procfd_path_echo,
                          sizeof ( procfd_path_echo ) - 1 );
    
    if ( path_len == -1 )
    { 
        perror("readlink");
        exit(EXIT_FAILURE);
    }
    procfd_path_echo [ path_len ] = '\0';

    return procfd_path_echo;
}
