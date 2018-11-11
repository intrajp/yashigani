/*
 *  cfg.h - configuration definition 
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

#ifndef YASHIGANI__CFG_H
#define YASHIGANI__CFG_H

struct yashigani_config
{
    /* mount-point to be checked */
    char mount_points [ 255 ];
};

/*  this is a pointer to the global configuration, it should be available
 *  once cfg_init() was called
 */
extern struct yashigani_config *yashigani_cfg;

/*  Initialize the configuration in yashigani. This method
 *  will read the default configuration file and call exit()
 *  if an error occurs.
 */
void cfg_init ( const char *file_name );

/* This function reads config file. */

void cfg_read ( const char *file_name, struct yashigani_config *cfg );

/*  This function frees the memory which had been allocated
 *  for config file.
 */
void cfg_clear (  );

#endif /* YASHIGANI__CFG_H */
