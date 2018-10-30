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
char path_name_ok [ PATH_MAX ];

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

    /* Loop while events can be read from fanotify file descriptor */
    for( ; ; )
    {
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
                ////////////////////////////////////////////
                /* check fd is executable or not */
                executable = check_executable ( metadata->fd , &info );
		printf ("executable:%d\n",executable);
		path_echo = get_path_name ( metadata->fd );
                /* calculate hash of path */
                sig_echo = calc_hash ( path_echo );

                /* setting deny if fd is executable */
		if ( executable )
                {
                    /* check with the white-list */
                    if ( search_sig_and_path ( sig_echo, path_echo ) )
		    {
                        printf("FAN_ALLOW\n");
                        response.response = FAN_ALLOW; /* allow if path is in the list */
                        strncpy ( path_name_ok , path_echo, PATH_MAX - 1 );
                        printf("path_name_ok:%s\n",path_name_ok);
		    }
		    else
		    {
                        printf("path_name_ok2:%s\n",path_name_ok);
                        if ( ( strcmp ( path_name_ok, "" )  != 0 ) || ( search_sig_and_path ( sig_echo, path_name_ok ) ) )
                        {
                            printf("FAN_ALLOW\n");
                            response.response = FAN_ALLOW; /* allow if path is not in the list */
                            printf("path_name_ok3:%s\n",path_name_ok);
                            path_name_ok [ 0 ]= '\0';
                        }
                        else
                        {
                            printf("FAN_DENY\n");
                            response.response = FAN_DENY; /* deny if path is in the list */
                        }
		    }
                }
		else
                    response.response = FAN_ALLOW; /* allow if not executable */
	        ////////////////////////////////////////////

                /* Handle open permission event */
                if ( metadata->mask & FAN_OPEN_PERM )
                {
                    printf("FAN_OPEN_PERM: ");
                    /* Allow file to be opened */
                    response.fd = metadata->fd;
                    //response.response = FAN_ALLOW;
                    write ( fd, &response,
                          sizeof ( struct fanotify_response ) );
                }
                /* Handle closing of writable file event */
                if ( metadata->mask & FAN_CLOSE_WRITE )
                    printf("FAN_CLOSE_WRITE: ");
                printf("File %s\n", path_echo);
                close ( metadata->fd );
            }
            /* Advance to next event */
            metadata = FAN_EVENT_NEXT ( metadata, len );
        }
    }
}
