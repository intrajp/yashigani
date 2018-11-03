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
    const char *sig_echo = "";

    puts("\n---- handdle_events ----\n");
    /* Loop while events can be read from fanotify file descriptor */
    for( ; ; )
    {
        //puts("---- for loop");
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
                /* check fd is executable or not */
                executable = check_executable ( metadata->fd , &info );
		path_echo = get_path_name ( metadata->fd );
                /* calculate hash of path */
                sig_echo = calc_hash ( path_echo );
                /* start checking white-list with the executables */
		if ( executable )
                {
                    /* check with the white-list ( bin path )*/
                    if ( ( path_bin [ 0 ] == '\0' ) && ( search_sig_and_path ( sig_echo, path_echo, "./white-list/bin_files" ) ) )
		    {
                        printf("FAN_ALLOW-1\n");
                        response.response = FAN_ALLOW;
                        strncpy ( path_bin , path_echo, PATH_MAX - 1 );
                        printf("path_bin:%s\n",path_bin);
		    }
		    else
		    {
                        printf("path_bin:%s\n",path_bin);
                        memset ( path_lib, '\0', PATH_MAX );

                        /* check with the white-list ( library path )*/
                        if ( strcmp ( path_bin, "/usr/bin/ls" ) == 0 ) 
                        {
                            strncpy ( path_lib , path_echo, PATH_MAX - 1 );
                            printf("check this path:%s\n",path_lib);
                            if ( search_sig_and_path ( sig_echo, path_echo, "./white-list/library-files/usr_bin_ls" ) )
                            {
                                response.response = FAN_ALLOW;
                                strncpy ( path_lib , path_echo, PATH_MAX - 1 );
                                printf("Found:%s\n",path_echo);
                            }
                            else
                            {
                                printf("Not found:%s\n", path_echo);
                                response.response = FAN_DENY;
                            }
		        }
                        else if ( strcmp ( path_bin, "/usr/bin/sed" ) == 0 ) 
                        {
                            strncpy ( path_lib , path_echo, PATH_MAX - 1 );
                            printf("check this path:%s\n",path_lib);
                            if ( search_sig_and_path ( sig_echo, path_echo, "./white-list/library-files/usr_bin_sed" ) )
                            {
                                strncpy ( path_lib , path_echo, PATH_MAX - 1 );
                                printf("Found:%s\n",path_echo);
                                response.response = FAN_ALLOW;
                            }
                            else
                            {
                                printf("Not found:%s\n", path_echo);
                                response.response = FAN_DENY;
                            }
                        }
                        else
                        {
                            response.response = FAN_DENY;
                        }
                    }
                }
		else
                    response.response = FAN_ALLOW; /* allow if not executable */
	        ////////////////////////////////////////////

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
            /* Advance to next event */
            metadata = FAN_EVENT_NEXT ( metadata, len );
        }
    }
}
