/*
 *  yashigani.c - function
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

#include <limits.h>
#include <string.h>
#include "common.h"

char path_bin [ PATH_MAX ];
char path_lib [ PATH_MAX ];

/* Read all available fanotify events from the file descriptor 'fd' */

void handle_events ( int fd )
{
    const struct fanotify_event_metadata *metadata;
    struct fanotify_event_metadata buf [ 200 ];
    ssize_t len;
    struct fanotify_response response;
    struct stat info; /* for check_executable() */
    int executable = 0; /* for check_executable() */
    executable = 0; /* setting no executable */
    const char *path_echo = "";
    const char *hash_echo = "";
    /* meaning reached EOF */
    int path_ok = -1;

    puts("\n---- handdle_events ----\n");
    /* Loop while events can be read from fanotify file descriptor */
    for( ; ; )
    {
        puts("for loop");
        /* Read some events */
        len = read ( fd, ( void * ) &buf, sizeof ( buf ) );
        if ( len == -1 && errno != EAGAIN )
        {
            perror("read");
            exit(EXIT_FAILURE);
        }
        /* Check if end of available data reached */
        if ( len <= 0 )
            break;
        /* Point to the first event in the buffer */
        metadata = buf;
        /* Loop over all events in the buffer */
        while ( FAN_EVENT_OK ( metadata, len ) )
        {
            puts("while loop");
            /* Check that run-time and compile-time structures match */
            if ( metadata->vers != FANOTIFY_METADATA_VERSION )
            {
                fprintf(stderr,
                    "Mismatch of fanotify metadata version.\n");
                exit(EXIT_FAILURE);
            }
            /* metadata->fd contains either FAN_NOFD, indicating a
               queue overflow, or a file descriptor (a nonnegative
               integer). Here, we simply ignore queue overflow. */

            if ( metadata->fd >= 0 )
            {
                printf("metadata->fd:%d\n",metadata->fd);
                /* check fd is executable or not */
                executable = check_executable ( metadata->fd , &info );
                /* get path from fd */
		path_echo = get_path_name ( metadata->fd );
                /* calculate hash of path */
                hash_echo = calc_hash ( path_echo );
                /* start checking white-list with the executables */
		if ( executable )
                {
                    puts("executable");
                    /* check with the white-list ( bin path ) */
                    strncpy ( path_bin , path_echo, PATH_MAX - 1 );
                    printf("----path_bin:%s\n",path_bin);
                    if ( ( path_bin [ 0 ] == '/' ) && ( strstr ( path_bin, "/usr/lib") == NULL ) )
                    {
                        printf("I try to find the path and hash.'\n\n");
                        while ( 1 )
                        {
                            path_ok = search_path_and_hash ( path_echo, hash_echo, "./white-list/bin-files/list_bin" );
                            /* break if matched ( 0 ), come to an EOF ( -1 ) or hash did not match (security bleach) ( 2 ) */
                            if ( ( path_ok == 0 ) || ( path_ok == -1 ) || ( path_ok == 2 ) )
                                break;
                        }
                        if ( path_ok == -1 )
                        {
                            response.response = FAN_DENY;
                            strncpy ( path_bin , path_echo, PATH_MAX - 1 );
                            printf("Couldn't find %s in bin file.\n",path_bin);
                            /* initialize */
                            memset ( path_bin, '\0', PATH_MAX );
                            memset ( path_lib, '\0', PATH_MAX );
                        }
                        else if ( path_ok == 2 )
                        {
                            printf("search_path_and_hash says 'Path matched but hash did not matched.' \n\
                            This could be a security bleach, I deny to open this file.'\n");
                            response.response = FAN_DENY;
                            strncpy ( path_bin , path_echo, PATH_MAX - 1 );
                            /* initialize */
                            memset ( path_bin, '\0', PATH_MAX );
                            memset ( path_lib, '\0', PATH_MAX );
                        }
                        else if ( path_ok == 0 )
		        {
                            printf("search_path_and_hash says 'Path and hash matched. Moving on to the library search.'\n");
                            response.response = FAN_ALLOW;
                            strncpy ( path_bin , path_echo, PATH_MAX - 1 );
                            printf("path_bin:%s\n",path_bin);
                            /* initialize */
                            memset ( path_lib, '\0', PATH_MAX );
		        }
                        else
                        {
                            printf("Not found:%s\n", path_echo);
                            response.response = FAN_DENY;
                            /* initialize */
                            memset ( path_bin, '\0', PATH_MAX );
                            memset ( path_lib, '\0', PATH_MAX );
                        }
                    }
                    if ( path_ok == 0 )
		    {
                        printf("I've checked this path and hash, so I try to check library's path and hash.'\n\n");
                        printf("path_bin:%s\n",path_bin);
                        /* initialize */
                        memset ( path_lib, '\0', PATH_MAX );

                        /* check with the white-list ( library path ) */
                        if ( strstr ( path_bin, "/usr/bin/" ) != 0 ) 
                        {
                            strncpy ( path_lib , path_echo, PATH_MAX - 1 );
                            printf("check this path:%s\n",path_lib);
                            /* check with the white-list ( bin path ) */
                            while ( 1 )
                            {
                                path_ok = search_path_and_hash ( path_echo, hash_echo, "./white-list/library-files/usr_bin" );
                                printf ("path_ok:%d\n",path_ok);
                                /* break if matched ( 0 ) or come to an EOF ( -1 ) */
                                if ( ( path_ok == 0 ) || ( path_ok == -1 ) || ( path_ok == 2 ) )
                                    break;
                            }
                            if ( path_ok == -1 )
                            {
                                response.response = FAN_DENY;
                                strncpy ( path_lib , path_echo, PATH_MAX - 1 );
                                printf("Couldn't find %s in lib file.\n",path_lib);
                                /* initialize */
                                memset ( path_bin, '\0', PATH_MAX );
                                memset ( path_lib, '\0', PATH_MAX );
                            }
                            else if ( path_ok == 2 )
                            {
                                printf("search_path_and_hash says 'Path matched but hash did not matched.' \n\
                                This could be a security bleach, I deny to open this file.'\n");
                                response.response = FAN_DENY;
                                strncpy ( path_lib , path_echo, PATH_MAX - 1 );
                                /* initialize */
                                memset ( path_bin, '\0', PATH_MAX );
                                memset ( path_lib, '\0', PATH_MAX );
                            }
                            else if ( path_ok == 0 )
		            {
                                printf("search_path_and_hash says 'Path and hash matched'\n");
                                response.response = FAN_ALLOW;
                                strncpy ( path_lib , path_echo, PATH_MAX - 1 );
                                printf("path_lib:%s\n",path_lib);
                                /* initialize */
                                memset ( path_bin, '\0', PATH_MAX );
                                memset ( path_lib, '\0', PATH_MAX );
		            }
                            else
                            {
                                printf("Not found:%s\n", path_echo);
                                response.response = FAN_DENY;
                            }
		        }
                        else if ( path_ok == -1 )
                        {
                            printf("Failed to find path and hash:%s\n", path_echo);
                            response.response = FAN_DENY;
                            /* initialize */
                            memset ( path_bin, '\0', PATH_MAX );
                            memset ( path_lib, '\0', PATH_MAX );
                        }
                    }
                }
		else
                {
                    /*puts("not executable");*/
                    /*printf("path_echo:%s\n",path_echo);*/
                    response.response = FAN_ALLOW; /* allow if not executable */
                }

                /* Handle open permission event */
                if ( metadata->mask & FAN_OPEN_PERM )
                {
                    /* Allow file to be opened */
                    response.fd = metadata->fd;
                    write ( fd, &response,
                          sizeof ( struct fanotify_response ) );
                }
                /* Handle closing of writable file event */
                if ( metadata->mask & FAN_CLOSE_WRITE )
                    printf("FAN_CLOSE_WRITE: ");
                close ( metadata->fd );
            }
            /* initialize */
            memset ( path_bin, '\0', PATH_MAX );
            memset ( path_lib, '\0', PATH_MAX );
            /* Advance to next event */
            metadata = FAN_EVENT_NEXT ( metadata, len );
        }
    }
}
