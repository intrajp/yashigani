/*
 *  main.c - main function
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
#include <poll.h>
#include <string.h>
#include "common.h" 

extern char path_bin [ PATH_MAX ];
extern char path_lib [ PATH_MAX ];

int main ( int argc, char *argv [ ] )
{
    memset ( path_bin, '\0', PATH_MAX );
    memset ( path_lib, '\0', PATH_MAX );
    char buf;
    int fd, poll_num;
    nfds_t nfds;
    struct pollfd fds [ 2 ];

    /* Check mount point is supplied */
    if ( argc != 2 )
    {
        fprintf(stderr, "Usage: %s MOUNT\n", argv[0]);
        exit ( EXIT_FAILURE );
    }
    printf("Press enter key to terminate.\n");
    /* Create the file descriptor for accessing the fanotify API */

    fd = fanotify_init ( FAN_CLOEXEC | FAN_CLASS_CONTENT | FAN_NONBLOCK,
                              O_RDONLY | O_LARGEFILE );
    if ( fd == -1 )
    {
        perror ( "fanotify_init" );
        exit ( EXIT_FAILURE );
    }

    /* Mark the mount for:
        - permission events before opening files
        - notification events after closing a write-enabled
          file descriptor */
    if ( fanotify_mark ( fd, FAN_MARK_ADD | FAN_MARK_MOUNT,
                FAN_OPEN_PERM | FAN_CLOSE_WRITE, AT_FDCWD,
                argv [ 1 ] ) == -1 )
    {
        perror ( "fanotify_mark" );
        printf("debug - errno:%d\n",errno);
        exit ( EXIT_FAILURE );
    }
    /* Prepare for polling */
    nfds = 2;
    /* Console input */
    fds [ 0 ].fd = STDIN_FILENO;
    fds [ 0 ].events = POLLIN;
    /* Fanotify input */
    fds [ 1 ].fd = fd;
    fds [ 1 ].events = POLLIN;

    /* This is the loop to wait for incoming events */
    printf("Listening for events.\n");
    while ( 1 )
    {
        poll_num = poll ( fds, nfds, -1 );
        if ( poll_num == -1 )
        {
            if ( errno == EINTR )   /* Interrupted by a signal */
                continue;           /* Restart poll() */
            perror ( "poll" );         /* Unexpected error */
            exit ( EXIT_FAILURE );
        }
        if ( poll_num > 0 )
        {
            if ( fds [ 0 ].revents & POLLIN )
            {
                /* Console input is available: empty stdin and quit */
                while ( read ( STDIN_FILENO, &buf, 1 ) > 0 && buf != '\n' )
                    continue;
                break;
            }
            if ( fds [ 1 ].revents & POLLIN )
	    {
                /* Fanotify events are available */
                handle_events( fd );
	    }
        }
    }
    printf("Listening for events stopped.\n");
    exit ( EXIT_SUCCESS );
}
