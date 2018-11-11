/*
 *  line_data.h - object definition 
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

#ifndef YASHIGANI__LINE_DATA_H
#define YASHIGANI__LINE_DATA_H

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fp;
FILE *fp2;

typedef struct line_data
{
    const char _line [ PATH_MAX ];
    struct line_data *next;
} node;

/* pointer to the global data, it should be available
 * once yashigani_init () was called.
 */
extern struct line_data *yashigani_bin_obj;
extern struct line_data *yashigani_lib_obj;

node *allocate_mem_to_one_node ( void );
int delete_obj ( node **obj );
int init_list ( node **obj );
int insert_node_top_of_the_list ( node **obj, char *line );
void set_list ( node *obj, char *line, node *obj_next );

#endif /* YASHIGANI_LINE_DATA_H */
