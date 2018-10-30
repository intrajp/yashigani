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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h> 
#include <unistd.h> 

int main ( int argc, char *argv [ ] )
{
    const char *path = "/bin/ab";
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
