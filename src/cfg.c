/*
 *  cfg.c - configuration functions 
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
#include "cfg.h"

/* should be set to NULL, this will be checked in cfg_init() */
struct yashigani_config *yashigani_cfg = NULL;

const char msg_cfg_read [ 36 ] = "cfg_read was called by SIG_VALUE !\n";
const char msg_cfg_read_ok [ 40 ] = "file value was reloaded by SIG_VALUE !\n";

/* the maximum line length in the configuration file */
#define MAX_LINE_LENGTH    4096

/* the delimiters of tokens */
#define TOKEN_DELIM " \t\n\r"

/* set the configuration information to the defaults after memory is allocated*/

void cfg_defaults ( struct yashigani_config *cfg )
{
    memset ( cfg, 0, sizeof ( struct yashigani_config ) );
    if ( cfg == NULL )
    {
        printf("cfg.c: cfg_defaults() failed to memset");
        exit(EXIT_FAILURE);
    }
    printf("cfg_defaults was called !\n");

    strncpy ( cfg->mount_points, "/", 255 );
}

/* check that the condition is true and otherwise log an error
   and bail out */
/* This function is only called from this file, so set static.*/

static inline void check_argumentcount ( const char *file_name, int lnr,
                                       const char *keyword, int condition )
{
    if ( !condition )
        printf("%s:%d: %s: wrong number of arguments",
                file_name, lnr, keyword); 
    /*
    {
        segatex_msg ( LOG_ERR, "%s:%d: %s: wrong number of arguments",
                file_name, lnr, keyword );
        exit ( EXIT_FAILURE );
    }
    */
}

/* This function works like strtok() except that the original string is
 *  not modified and a pointer within str to where the next token begins
 *  is returned (this can be used to pass to the function on the next
 *  iteration). If no more tokens are found or the token will not fit in
 *  the buffer, NULL is returned. 
*/
/* This function is only called from this file, so set static.*/

static char *get_token ( char **line, char *buf, size_t buflen )
{
    /* printf("get_token was called !\n"); */
    /* printf("buf:%s\n", buf); */

    size_t len;

    if ( ( line == NULL ) || ( *line == NULL ) || ( **line == '\0' ) || ( buf == NULL ) )
        return NULL;

    /*printf("get_token 2\n"); */

    /* find the beginning and length of the token */

    *line += strspn ( *line, TOKEN_DELIM );
    len = strcspn ( *line, TOKEN_DELIM );

    /* check if there is a token */

    if ( len == 0 )
    {
        *line = NULL ;
        return NULL;
    }

    /* limit the token length */

    if ( len >= buflen )
        len = buflen - 1;

    /* printf("len is %d\n",len); */
    
    /* copy the token */
    strncpy ( buf, *line, len );
    buf [ len ] = '\0';

    /* printf("buf is %s\n",buf); */
    
    /* skip to the next token */
    *line += len;
    *line += strspn ( *line, TOKEN_DELIM );

    /* return the token */
    return buf;
}

/* This function is only called from this file, so set static.*/

static int get_int ( const char *file_name, int lnr,
                    const char *keyword, char **line )
{
    char token [ 32 ];
    /* printf("get_init was called !\n"); */
    /* printf("file_name is %s\n", file_name); */
    /* printf("lnr is %d\n", lnr); */
    /* printf("keyword is %s\n", keyword); */
    /* printf("line is %s\n", *line); */
    
    check_argumentcount ( file_name, lnr, keyword,
                    get_token ( line, token, sizeof ( token ) ) != NULL );

    /* TODO: replace with correct numeric parse */

    return atoi ( token );
}

/* This function is only called from this file, so set static.*/

static void get_eol ( const char *file_name, int lnr,
                    const char *keyword, char **line )
{
    printf("get_eol was called !\n");

    if ( ( line != NULL ) && ( *line != NULL ) && ( **line != '\0' ) )
        printf("%s:%d: %s: too may arguments", file_name, lnr, keyword );
    /*
    {
        segatex_msg ( LOG_ERR, "%s:%d: %s: too may arguments", file_name, lnr, keyword );
        exit ( EXIT_FAILURE );
    }
    */
}

/* read configuration file of segatexd */

void cfg_read ( const char *file_name, struct yashigani_config *cfg )
{
    char line_break [ 5 ]= "\n" ;
    char mount_points_str [ PATH_MAX ] = "";
    char mount_points_pre [ 22 ] = "cfg->mount_points is ";

    /*if signal was caught, printf is unsafe, so change it to write*/
    /*
    if ( SIG_VALUE == 2 )
    {
        write ( STDOUT_FILENO, msg_cfg_read, sizeof ( msg_cfg_read ) - 1 );
    }
    else
    {
        printf("cfg_read was called !\n");
    }
    */

    FILE *fp;
    int fd, rc, lnr = 0;
    struct stat st;
    char linebuf [ MAX_LINE_LENGTH ];
    char *line;
    char keyword [ 32 ];
    int i;

    /* open the file */
    rc = open ( file_name, O_NOFOLLOW | O_RDONLY );
    if ( rc < 0 ) {
        if ( errno != ENOENT )
	/*
        {
            segatex_msg ( LOG_ERR, "Error opening %s (%s)",
                file_name, strerror ( errno ) );
            exit ( EXIT_FAILURE );
        }
	*/
        /* segatex_msg ( LOG_WARNING,
            "Config file %s doesn't exist", filename ); */
        {
            printf("Error opening %s (%s)",
                file_name,strerror(errno));
	    printf("Config file %s doesn't exist.\n",file_name);
            exit ( EXIT_FAILURE );
	}
    }

    fd = rc;

    /* check the file's permissions: owned by root, not world writable,
     * not symlink.
     */
    /* segatex_msg ( LOG_DEBUG, "Config file %s opened for parsing", filename ); */
    printf("Config file %s opened for parsing\n",file_name);

    if ( fstat ( fd, &st ) < 0) {
        /* segatex_msg ( LOG_ERR, "Error fstat'ing %s (%s)",
            filename, strerror ( errno ) ); */
        printf("Error fstat'ing %s (%s)\n",file_name,strerror(errno)); 
        close ( fd );
        exit ( EXIT_FAILURE );
    }
    if ( st.st_uid != 0 ) {
        /* segatex_msg ( LOG_ERR, "Error - %s isn't owned by root", filename );*/
        printf("Error - %s isn't owned by root\n",file_name); 
        close ( fd );
        exit ( EXIT_FAILURE );
    }
    if ( ( st.st_mode & S_IWOTH ) == S_IWOTH ) {
        /* segatex_msg ( LOG_ERR, "Error - %s is world writable", filename ); */
        printf("Error - %s is world writable",file_name );
        close ( fd );
        exit ( EXIT_FAILURE );
    }
    /* using macro to check file*/
    if ( !S_ISREG ( st.st_mode ) ) {
        /* segatex_msg ( LOG_ERR, "Error - %s is not a regular file", filename ); */
        printf("Error - %s is not a regular file",file_name );
        close ( fd );
        exit ( EXIT_FAILURE );
    }

    /* open config file */
    if ( ( fp = fopen ( file_name,"r" ) ) == NULL )
    {
        /* segatex_msg ( LOG_ERR, "cannot open config file (%s): %s", filename, strerror ( errno ) ); */
        printf("cannot open config file (%s): %s", file_name, strerror ( errno ) );
        close ( fd );
        exit ( EXIT_FAILURE );
    }
    /* read file and parse lines */
    while ( fgets ( linebuf, sizeof ( linebuf ), fp ) != NULL )
    {
        lnr++;
        line = linebuf;
        /* strip newline */
        i = ( int ) strlen ( line );
        if ( ( i <= 0 ) || ( line [ i - 1 ] != '\n' ) )
        {
            /* segatex_msg ( LOG_ERR, "%s:%d: line too long or last line missing newline", filename, lnr ); */
            printf("%s:%d: line too long or last line missing newline", file_name, lnr);
            close ( fd );
            exit ( EXIT_FAILURE );
        }
        line [ i - 1 ] = '\0';
        /* ignore comment lines */
        if ( line [ 0 ] == '#' )
            continue;
        /* strip trailing spaces */
        for ( i--; ( i > 0 ) && isspace ( line [ i - 1 ] ); i-- )
            line [ i - 1 ] = '\0';

        /* get keyword from line and ignore empty lines */
        if ( get_token ( &line, keyword, sizeof ( keyword ) ) == NULL )
            continue;
        printf("keyword is %s\n",keyword);
        /* runtime options */
        if ( strcmp ( keyword, "mount_options" ) == 0 )
        {
            strncpy ( cfg->mount_points, line, 255 );
            get_eol ( file_name, lnr, keyword, &line );
        }
        /* fallthrough */
        else
        {
            /* segatex_msg ( LOG_ERR, "%s:%d: unknown keyword: '%s'", filename, lnr, keyword ); */
            printf("%s:%d: unknown keyword: '%s'", file_name, lnr, keyword );
            close ( fd );
            exit ( EXIT_FAILURE );
        }
    }
    /*setting int to char string*/
    strncpy ( mount_points_str, mount_points_pre, 22 );
    strcat ( mount_points_str, cfg->mount_points );
    strcat ( mount_points_str, line_break );


    /*if signal was caught, printf is unsafe, so change it to write*/
    //show file contents
    //
    //XXX have to fix, when reload is done, file name still the same...
    //
    /*
    if ( SIG_VALUE == 2 )
    {
        write ( STDOUT_FILENO, msg_cfg_read_ok, sizeof ( msg_cfg_read_ok ) );
        write ( STDOUT_FILENO, struct_str, sizeof ( struct_str ) - 1 );
        write ( STDOUT_FILENO, filename_str, sizeof ( filename_str ) - 1 );
    }
    else
    {
        printf ( "cfg->threads is %d\n", cfg->threads );
        printf ( "cfg->send_file is %s\n", cfg->send_file );
    }
    */
    /* we're done reading file, close */
    fclose ( fp );
}

/* This function is called anywhere, so not set static.*/

void cfg_init ( const char *file_name )
{
    printf("cfg_init was called !\n");
    /* check if we were called before */
    if ( yashigani_cfg != NULL )
        printf("cfg_init() may only be called once.\n");
    /*
    {
        segatex_msg ( LOG_CRIT, "cfg_init() may only be called once" );
        exit ( EXIT_FAILURE );
    }
    */
    /* allocate the memory (this memory is not freed anywhere, it's ok, outside this socpe, it's NULL) */
    yashigani_cfg = (struct yashigani_config * ) malloc ( sizeof ( struct yashigani_config ) );
    /* free(segatexd_cfg); */
    if ( yashigani_cfg == NULL )
        printf("malloc() failed to allocate memory.\n");
    /*
    {
        segatex_msg ( LOG_CRIT, "malloc() failed to allocate memory" );
        exit ( EXIT_FAILURE );
    }
    */

    /* clear configuration */
    cfg_defaults ( yashigani_cfg );
    //for debug
    //free(segatexd_cfg);
    /* read configfile */
    cfg_read ( file_name, yashigani_cfg );
    printf("yashigani_cfg:%p\n",yashigani_cfg);
}

/*
 * after outside cfg_init(), yashigani_cfg should be null, so, no need this function? 
*/

void cfg_clear ( )
{
    printf("freeing the config pointer, cause 've already read stuff -- yashigani_cfg:%p\n",yashigani_cfg);
    free ( yashigani_cfg );
    yashigani_cfg = NULL;
    printf("should be NULL -- yashigani_cfg:%p\n",yashigani_cfg);
}
