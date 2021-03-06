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
#include <ctype.h> /* for is_space */
#include <getopt.h>
#include <poll.h>
#include "main.h" 

extern char path_bin [ PATH_MAX ];
extern char path_lib [ PATH_MAX ];

/* brief Print help for this application */

void usage ( void )
{
    printf("\n Usage: %s [OPTIONS]\n\n", app_name);
    printf("  Options:\n");
    printf("   -R --reload-list          Reload from white-list files\n");
    printf("   -p --print-list           print present white-list on memory\n");
    printf("   -h --help                 Print this help\n");
    printf("\n");
}

int main ( int argc, char *argv [ ] )
{
    int value, option_index;
    int RELOAD = 0;
    int PRINT = 0;
    memset ( path_bin, '\0', PATH_MAX );
    memset ( path_lib, '\0', PATH_MAX );
    char buf;
    int fd, poll_num;
    nfds_t nfds;
    struct pollfd fds [ 2 ];

    static struct option long_options [ ] = {
        { "reload-list", no_argument,     0, 'r' },
        { "print-list", no_argument,     0, 'r' },
        { "help",      no_argument,       0, 'h' },
        { NULL,        0,                 0,  0  }
    };

    /* Try to process all command line arguments */
    while ( ( value = getopt_long(argc, argv, "Rph", long_options, &option_index ) ) != - 1 ) {
        switch ( value ) {
            case 'R':
		RELOAD = 1;
                break;
            case 'p':
		PRINT = 1;
                break;
            case 'h':
                usage ( );
                return EXIT_SUCCESS;
                case '?':
                usage ( );
                return EXIT_FAILURE;
            default:
                break;
        }
    }
    if ( RELOAD == 1 )
    {
        yashigani_init ( 99 );
        puts ( "\n\nData had been restored from files." );
        exit ( EXIT_SUCCESS );
    }

    /* Check mount point is supplied */
    if ( argc != 2 )
    {
        /* fprintf(stderr, "Usage: %s MOUNT\n", argv[0]); */
	usage();
        exit ( EXIT_FAILURE );
    }

    /* initialize yashigani stuff */
    yashigani_init ( 1 );

    if ( PRINT == 1 )
    {
        puts ( "\nPrint out present white-list on memory." );
        print_list ( &yashigani_bin_obj );
        puts ( "--------" );
        print_list ( &yashigani_lib_obj );
        exit ( EXIT_SUCCESS );
    }

    printf("Press enter key to terminate.\n");
    /* Create the file descriptor for accessing the fanotify API */

    fd = fanotify_init ( FAN_CLOEXEC | FAN_CLASS_CONTENT | FAN_NONBLOCK,
                              O_RDONLY | O_LARGEFILE );
    if ( fd == -1 )
    {
        perror ( "fanotify_init" );
        free_yashigani_obj ( );
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
        free_yashigani_obj ( );
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
    /* printf("Listening for events.\n"); */
    while ( 1 )
    {
        poll_num = poll ( fds, nfds, -1 );
        if ( poll_num == -1 )
        {
            if ( errno == EINTR )   /* Interrupted by a signal */
                continue;           /* Restart poll() */
            perror ( "poll" );         /* Unexpected error */
            free_yashigani_obj ( );
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
                /*puts("handle_events ( fd ) "); */
                handle_events( fd );
	    }
        }
    }
    printf("Listening for events stopped.\n");
    free_yashigani_obj ( );
    exit ( EXIT_SUCCESS );
}
