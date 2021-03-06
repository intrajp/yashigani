/*
 *  main.h - main definition 
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

#ifndef YASHIGANI__MAIN_H
#define YASHIGANI__MAIN_H

#include "common.h" 

static char *app_name = "yashigani";

/*
 * usage 
 *
 * This function prints help 
 *
 * Caller main.c
 *
 * Calls
 */
void usage ( void );

/*
 * print_list 
 *
 * This function prints object ingredients 
 *
 * Caller
 *
 * Calls
 */
void print_list ( node **obj );

#endif /* YASHIGANI_MAIN_H */
