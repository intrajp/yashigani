/*
 *  common.h - common definition 
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
char buff [ 4096 + 1 ];

int search_sig_and_path ( const char *sig, const char *path, const char *searchfile )
{

    FILE *fp;
    int lnr = 0;
    struct stat;
    char linebuf [ 4096 ];
    char *line;
    int i;

    /* open file */
    if ( ( fp=fopen ( searchfile, "r" ) ) == NULL )
    {
        printf("can't open file (%s): %s\n",searchfile,strerror(errno));
        exit ( EXIT_FAILURE );
    }

    /* read file and parse lines */
    while ( fgets ( linebuf, sizeof ( linebuf ), fp ) != NULL )
    {
        lnr++;
        line = linebuf;
        /* strip newline */
        i = ( int ) strlen ( line );
        if ( ( i <= 0 ) || ( line [ i - 1 ] != '\n' ) )
        {
            printf("%s:%d: line too long or last line missing newline\n",searchfile,lnr);
            exit ( EXIT_FAILURE );
        }
        /* this for compare rightly */
        line [ i - 1 ] = '\0';
        if ( strcmp ( line, path ) == 0 )
	{
            return ( 1 );
        }
        line [ i - 1 ] = '\0';
        /* strip trailing spaces */
        for ( i--; ( i > 0 ) && isspace ( line [ i -1 ] ) ; i-- )
            line [ i -1 ] = '\0';
    }
    /* after reading all lines, close the file pointer */
    fclose ( fp );

    return ( 0 );
}

int check_executable ( int fd, struct stat *buf )
{
    if ( fstat ( fd, buf ) != 0 )
    {
        perror ( "fstat() error" );   
        return ( -1 );
    }
    else
    {
        if ( buf->st_mode & S_IXUSR )
        {
            //puts ("executable");   
	    return ( 1 );
        }
	////else
            ////puts ("no");   
/*
        puts ("fstat() returned:");   
        printf ("inode:%d\n",(int)buf->st_ino);   
        printf ("dev id:%d\n",(int)buf->st_dev);   
        printf ("mode:%ox\n",buf->st_mode);   
        printf ("links:%d\n",(int)buf->st_nlink);   
        printf ("uid:%d\n",(int)buf->st_uid);   
        printf ("gid:%d\n",(int)buf->st_gid);   
*/
    }

    return ( 0 );
}

const char *show_hex_string ( unsigned char *hex, size_t n )
{
    char buff2 [ n + 1 ];
    for ( size_t i = 0; i < n; i++ )
    {
	snprintf ( buff2, n, "%02x", hex [ i ] );
	strncpy ( buff, buff2, PATH_MAX );
    }
    return buff;
}

const char *calc_hash ( const char *path )
{
    unsigned char hash [ SHA256_DIGEST_LENGTH ];
    SHA256 ( ( unsigned char * ) path, strlen ( path ), hash );
    show_hex_string( hash, SHA256_DIGEST_LENGTH );

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
