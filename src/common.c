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

#include <string.h>
#include <limits.h>
#include <time.h> 
#include <unistd.h> 
#include "common.h"

char procfd_path_echo [ PATH_MAX ];

int  check_executable ( int fd, struct stat *buf )
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
            puts ("executable");   
	    return ( 1 );
        }
	else
            puts ("no");   
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
