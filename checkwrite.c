/****************************************/
/* This is the checkwrite.c module for 	*/
/* lsat program. This module checks for */
/* world/group writable files and dirs  */
/* and prints them to the outfile.      */
/*					*/
/* NOTE: this will miss some files if   */
/* run as a non-root user. Not sure if  */
/* you should run as root. :)		*/
/*					*/
/* See lsatmain.c for more details.	*/
/****************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int checkwrite(filename, distribution, verbose, html)
const char *const filename;
int distribution;
int verbose;
int html;
{
    const char * tempfile =NULL;
    const char * shellcode=NULL;
    const char * header   =NULL;

    if (verbose >= 0)
    {
    	printf(" Running checkwrite module...\n");
    }

    if (verbose > 0)
    {
        printf(" Creating list of world writable files...\n");
        if (distribution != 5)
        {
            printf(" Note: we are not traversing \"other\" filesystems.\n");
        }
    }

    
    /*
    Generate a list of world/group writables from / .
    -mount is used so that we don't traverse other
    filesystems. We use -mount instead of -xdev for
    compatibility with other versions of find.
    Note that Mac OSX does not have -mount, and the man
    claims that -x is the old -xdev... I am not so sure
    about that... WTF were they smoking?
    */ 
    
    tempfile = "/tmp/lsat1.lsat";
    if (distribution == 5)
    { 
        /* OSX */
        shellcode = "find / -type f -perm -o+w 2>/dev/null |tee >/tmp/lsat1.lsat";
    }
    else
    {
        shellcode= "find / -mount -type f -perm -2  2>/dev/null | tee >/tmp/lsat1.lsat";
    }
    header   = "This is a list of world writable files\n";
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
	/* something went wrong */
	perror(" Creation of list failed.");
	return (-1);
    }

    if (verbose > 0)
    {
        printf(" Creating list of world writable directories...\n");
        printf(" Note: we are not traversing \"other\" filesystems.\n");
    }

    tempfile = "/tmp/lsat1.lsat";
    if (distribution == 5)
    {      
        /* OSX */
        shellcode = "find / -type f -perm -g+w 2>/dev/null |tee >/tmp/lsat1.lsat";
    }
    else
    {
        shellcode= "find / -mount -type f -perm -20  2>/dev/null | tee > /tmp/lsat1.lsat";
    }
    header   = "This is a list of group writable files\n";
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.");
        return (-1);
    }

    if (verbose > 0)
    {
        printf(" Creating list of group writable directories...\n");
        printf(" Note: we are not traversing \"other\" filesystems.\n");
    }


    
    /* and now the directories */
    tempfile ="/tmp/lsat1.lsat";
    if (distribution == 5)
    {      
        /* OSX */
        shellcode = "find / -type d -perm -o+w 2>/dev/null |tee >/tmp/lsat1.lsat";
    }
    else
    { 
        shellcode="find / -mount -type d  -perm -2  2>/dev/null | tee >/tmp/lsat1.lsat";
    }
    header   ="List of group writable directories:\n";
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
	/* something went wrong */
	perror(" Creation of list failed.");
	return (-1);
    }

    tempfile ="/tmp/lsat1.lsat";
    if (distribution == 5)
    {      
        /* OSX */
        shellcode = "find / -type d -perm -o+w 2>/dev/null |tee >/tmp/lsat1.lsat";
    }
    else
    {
        shellcode = "find / -mount -type d  -perm -20 2>/dev/null | tee >/tmp/lsat1.lsat";
    }
    header   ="List of world writable directories:\n";
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.");
        return (-1);
    }


    if (verbose > 0)
    {
        printf(" Finished in checkwrite module.\n");
    }

    return(0);
}
