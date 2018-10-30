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

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fanotify.h>
#include <sys/stat.h> /* for check executable */
#include <sys/types.h> /* for check executable */
#include <unistd.h>

//extern int inode;   
//extern int inode_former;   

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
//int calc_hash ( const char *path );
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
 * search_sig_and_path 
 *
 * This function searches signiture and path from file 
 *
 * if matches, return 1, if not return 0
 *
 * Caller yashigani.c
 *
 * Calls
 */
int search_sig_and_path ( const char *sig, const char *path );

const char *showHexString( unsigned char *hex, size_t n );
