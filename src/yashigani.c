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

//#define _GNU_SOURCE     /* Needed to get O_LARGEFILE definition */
#include <limits.h>
#include "common.h"

/* Read all available fanotify events from the file descriptor 'fd' */

void handle_events ( int fd )
{
    const struct fanotify_event_metadata *metadata;
    struct fanotify_event_metadata buf [ 200 ];
    ssize_t len;
    char path [ PATH_MAX ];
    ssize_t path_len;
    char procfd_path [ PATH_MAX ];
    struct fanotify_response response;
    /* Loop while events can be read from fanotify file descriptor */
    for( ; ; ) {
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
        while ( FAN_EVENT_OK(metadata, len ) )
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
                /* Handle open permission event */
                if ( metadata->mask & FAN_OPEN_PERM )
                {
                    printf("FAN_OPEN_PERM: ");
                    /* Allow file to be opened */
                    response.fd = metadata->fd;
                    response.response = FAN_ALLOW;
                    write ( fd, &response,
                          sizeof ( struct fanotify_response ) );
                }
                /* Handle closing of writable file event */
                if ( metadata->mask & FAN_CLOSE_WRITE )
                    printf("FAN_CLOSE_WRITE: ");
                /* Retrieve and print pathname of the accessed file */
                snprintf ( procfd_path, sizeof ( procfd_path ),
                         "/proc/self/fd/%d", metadata->fd );
                path_len = readlink ( procfd_path, path,
                                    sizeof ( path ) - 1 );
                if ( path_len == -1 )
                { 
                    perror("readlink");
                    exit(EXIT_FAILURE);
                }
                path [ path_len ] = '\0';
                printf("File %s\n", path);
                /* Close the file descriptor of the event */
                close ( metadata->fd );
            }
            /* Advance to next event */
            metadata = FAN_EVENT_NEXT ( metadata, len );
        }
    }
}
