/*
 *  load_data.c - read file and append to obj 
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
#include "common.h"

void load_data ( const char *file_name, node **obj )
{
    char *line = "";
    char linebuf [ PATH_MAX ];
    int lnr = 0;
    int i;
    /* opening file and get file pointers */
    if ( ( fp = fopen ( file_name, "r" ) ) == NULL )
    {
        printf("can't open file (%s): %s\n",file_name,strerror(errno));
        exit ( EXIT_FAILURE);
    }
    else
        printf("Opened file (%s)\n",file_name);
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
            printf("%s:%d: line too long or last line missing newline\n",file_name,lnr);
            exit ( EXIT_FAILURE);
        }
        /* this for compare rightly */
        line [ i - 1 ] = '\0';
        /* strip trailing spaces */
        for ( i--; ( i > 0 ) && isspace ( line [ i -1 ] ) ; i-- )
            line [ i -1 ] = '\0';
	printf ("%s\n",line);
	append_list ( obj, line );
    }
    /* after reading all lines, close the file pointer */
    fclose ( fp );
}
