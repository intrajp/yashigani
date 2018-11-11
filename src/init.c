/*
 *  init.c - init functions 
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

#include "common.h"
#include "init.h"

char procfd_path_echo [ PATH_MAX ];
char buff [ PATH_MAX ];

/* initialise each obj */
struct line_data *yashigani_bin_obj = &line_obj_raw;
struct line_data *yashigani_lib_obj = &line_obj_raw;

const char *searchfile = "/usr/share/yashiganid/list_bin";
const char *searchfile2 = "/usr/share/yashiganid/list_lib";

int create_yashigani_obj ( void )
{
    /* allocate the memory for each obj */
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

void yashigani_init ( int x )
{
    /* now, creating object if not data reload */
    if ( x != 99 )
        create_yashigani_obj ( );
    /* now, load data from file to obj */
    load_data ( searchfile, &yashigani_bin_obj );
    load_data ( searchfile2, &yashigani_lib_obj );
}
