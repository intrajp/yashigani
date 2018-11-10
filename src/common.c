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

/* line_obj_raw */
struct line_data line_obj_raw =
    {
        "yashigani object", /* each line */
        NULL /* next pointer */
    };

/* initialise */
struct line_data *yashigani_bin_obj = &line_obj_raw;
struct line_data *yashigani_lib_obj = &line_obj_raw;

int create_yashigani_obj ( void )
{
    /* allocate the memory */
    yashigani_bin_obj = ( struct line_data * ) malloc ( sizeof ( struct line_data ) );
    yashigani_lib_obj = ( struct line_data * ) malloc ( sizeof ( struct line_data ) );

    if ( ( yashigani_bin_obj == NULL ) || ( yashigani_lib_obj == NULL ) )
    {
        printf("malloc() failed to allocate memory\n");
        exit ( EXIT_FAILURE );
    }

    return ( 0 );
}

int free_yashigani_obj ( void )
{
    /* free list */
    free ( yashigani_bin_obj );
    yashigani_bin_obj = NULL;

    free ( yashigani_lib_obj );
    yashigani_lib_obj = NULL;

    /* clear list */
    clear_list ( &yashigani_bin_obj );
    yashigani_bin_obj = NULL;

    clear_list ( &yashigani_lib_obj );
    yashigani_lib_obj = NULL;

    return ( 0 );
}

void yashigani_init ( void )
{
    const char *searchfile = "./white-list/bin-files/list_usr_bin_sbin";
    const char *searchfile2 = "./white-list/library-files/list_lib";
    char *line = "";
    char linebuf [ PATH_MAX ];
    int lnr = 0;
    int i;

    /* opening file and get file pointers */
    if ( ( fp = fopen ( searchfile, "r" ) ) == NULL )
    {
        printf("can't open file (%s): %s\n",searchfile,strerror(errno));
        exit ( EXIT_FAILURE );
    }
    else
        printf("Opened file (%s)\n",searchfile);

    /* opening file and get file pointers */
    if ( ( fp2 = fopen ( searchfile2, "r" ) ) == NULL )
    {
        printf("can't open file (%s): %s\n",searchfile2,strerror(errno));
        exit ( EXIT_FAILURE );
    }
    else
        printf("Opened file (%s)\n",searchfile2);

    /* now, creating object */
    create_yashigani_obj ( );

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
	    free_yashigani_obj ( );
            exit ( EXIT_FAILURE );
        }
        /* this for compare rightly */
        line [ i - 1 ] = '\0';
        /* strip trailing spaces */
        for ( i--; ( i > 0 ) && isspace ( line [ i -1 ] ) ; i-- )
            line [ i -1 ] = '\0';
	printf ("%s",line);
	append_list ( &yashigani_bin_obj, line );
    }
    /* after reading all lines, close the file pointer */
    fclose ( fp );

    /* read file and parse lines */
    while ( fgets ( linebuf, sizeof ( linebuf ), fp2 ) != NULL )
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
            printf("%s:%d: line too long or last line missing newline\n",searchfile2,lnr);
	    free_yashigani_obj ( );
            exit ( EXIT_FAILURE );
        }
        /* this for compare rightly */
        line [ i - 1 ] = '\0';
        /* strip trailing spaces */
        for ( i--; ( i > 0 ) && isspace ( line [ i -1 ] ) ; i-- )
            line [ i -1 ] = '\0';
	printf ("%s",line);
	append_list ( &yashigani_lib_obj, line );
    }
    /* after reading all lines, close the file pointer */
    fclose ( fp2 );
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

int check_each_hash_and_path ( char **line, const char *path, const char *hash )
{
    /* puts("-- I am now in check_each_hash_and_path --"); */
    const char s [ 4 ] = "  \t"; /* this is the delimiter */
    char *token = NULL;
    int break_id = 1;

    /* get the first token */
    token = strtok ( *line, s );
    /* printf("token:%s\n",token); */
    /* reached EOF, so return 1 */
    if ( strcmp ( token, "EOF" ) == 0 )
    {
        break_id = 1;
        /* printf("path:%s\n",path); */
        /* printf("--break_id--:%d\n",break_id); */
        return ( break_id );
    }
    /* did not match, but try again */
    if ( strcmp ( token, path ) != 0 )
    {
        
        /* printf("tokn:%s\n",token); */
        /* printf("path:%s\n",path); */
        /* puts("Not match !!"); */
        break_id = 3;
        /* printf("--break_id--:%d\n",break_id); */
        //return ( break_id );
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

    if ( strcmp ( token, path ) == 0 )
    {
        /*puts("!! path matched !!"); */
        /* walk throuth other tokens */
        while ( token != NULL )
        {
            /* puts("--------- Now I check hash! --------"); */
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
                /* printf("path:%s\n",path); */
                /* printf("hash:%s\n",hash); */
                break_id  = 1;
                token = NULL;
                break;
            }
            /* Both path and hash matched, so return ( 0 ) */
            else if ( strcmp ( token, hash ) == 0 )
            {
	        /* printf("tokn:%s\n",token); */
                /* printf("hash:%s\n",hash); */
                /* puts("hash ( and path ) Match !!"); */
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
    }
    /* printf("-- break_id of check each path and hash --:%d\n",break_id); */
    return ( break_id );
}

int search_path_and_hash ( const char *path, const char *hash, node **obj )
{
    node *ptr_tmp = *obj;

    char line_pre [ PATH_MAX ];
    char *line = "";
    memset ( line_pre, '\0', PATH_MAX );
    int break_id = -1;
    int ret = -1;

    /* first object has "",so skipping. */
    if ( strcmp ( ptr_tmp->_line , "" ) == 0 )
        ptr_tmp = ptr_tmp->next;

    while ( ptr_tmp != NULL )
    {
	while ( ptr_tmp != NULL )
        {
            /* printf("^^^^^^^^^path:%s\n",path); */
            /* printf("^^^^^^^^^hash:%s\n",hash); */
            /* printf("ptr_tmp->line:%s\n",ptr_tmp->_line); */
            strncpy ( line_pre, ptr_tmp->_line, PATH_MAX );
	    line = line_pre;
            /* printf("line:%s\n",line); */
            break_id = ( check_each_hash_and_path ( &line, path, hash ) );
            ptr_tmp = ptr_tmp->next;
            /* reached EOF */
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
            {
                continue;
                ptr_tmp = ptr_tmp->next;
            }
        }
	/* printf("==================== ret:%d\n",ret); */
        if ( ( ret == 0 ) || ( ret == 1 ) || ( ret == 2 ) )
            break;
    }
    /* printf("====Now, I return with ret:%d\n",ret); */
    return ( ret );
}
