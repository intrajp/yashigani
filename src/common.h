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

#ifndef YASHIGANI__COMMON_H
#define YASHIGANI__COMMON_H

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fanotify.h>
#include <sys/stat.h> /* for check executable */
#include <sys/types.h> /* for check executable */
#include <unistd.h>

#define PROGRAM_VERSION 0 
#define PROGRAM_RELEASE 4 
#define PROGRAM_SUB_RELEASE 2

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

/*
 * handle_events
 *
 * This function handle events
 *
 * Caller main.c
 *
 * Calls
 */
void  handle_events ( int fd );

/*
 * check_executable 
 *
 * This function checks fd and return 1 
 * if it's executable, if not, return 0 
 *
 * Caller main.c
 *
 * Calls
 */
int  check_executable ( int fd, struct stat *buf );

/*
 * calc_hash 
 *
 * This function returns hash 
 *
 * Caller main.c
 *
 * Calls
 */
const char *calc_hash ( const char *path );

/*
 * get_path 
 *
 * This function returns file path from fd 
 *
 * Caller yashigani.c
 *
 * Calls
 */
const char *get_path_name ( int fd );

/*
 * search_path_and_hash 
 *
 * This function searches signiture and path from file given
 *
 * Caller yashigani.c
 *
 * Calls
 */
int search_path_and_hash ( const char *path, const char *hash, node **obj );

/*
 * show_hex_string 
 *
 */
const char *show_hex_string( unsigned char *hex, size_t n );

/*
 * check_each_path_and_hash 
 *
 */
int check_each_path_and_hash ( char **line, const char *path, const char *hash );


/*
 * load_data 
 *
 * This function loads data from white-list files to memory
 *
 * Caller common.c
 *
 * Calls none
 */
void load_data ( const char *file_name, node **obj );

/*
 * yashigani_init 
 *
 * This function creates object and load data from files.
 * If 99 is given, restore from file to data.
 *
 * Caller init.c
 *
 * Calls none
 */
void yashigani_init ( int );

int check_token ( int break_id, node *ptr_tmp, char *token, const char *path, const char *hash );
int create_yashigani_obj ( void );
int free_yashigani_obj ( void );
int init_list ( node **obj );
int search_from_list ( node **obj, node **obj2, const char *path, const char *hash );

#endif /* YASHIGANI_COMMON_H */
