/****************************************/
/* This is the checkdotfiles module for */
/* lsat program. This module checks for */
/* .forward and .exrc files and prints  */
/* them to the outfile.      		*/
/* 4/5/2002 Added checks for .rhost(s)  */
/* and .netrc files.			*/
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

int checkdotfiles(filename, verbose, html)
const char *const filename;
int verbose;
int html;
{

    const char * tempfile =NULL;
    const char * shellcode=NULL;
    const char * header   =NULL;


    if (verbose >= 0)
    {
        printf(" Running checkdotfiles module...\n");
    }


    if (verbose > 0)
    {
        printf(" Creating list of .exrc files...\n");
    }

    
    /* generate list of all files called .exrc on */
    /* system */
    tempfile = "/tmp/lsat1.lsat";
    shellcode = "find / -mount -name \'.exrc\' 2>/dev/null | tee > /tmp/lsat1.lsat";
    header = "This is a list of .exrc files found\n";

    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
	/* something went wrong */
	perror(" Creation of .exrc list failed.");
	return(-1);
    }


    if (verbose > 0)
    {
        printf(" Creating list of .forward files...\n");
    }


    /* generate list of .forward files... */

    tempfile = "/tmp/lsat1.lsat";
    shellcode = "find / -mount -find \'.forward\' 2>/dev/null |tee > /tmp/lsat1.lsat";
    header = "This is a list of .forward files found on the system:\n";
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
	/* something went wrong */
 	perror(" Creation of .forward list failed.");
	return(-1);
    }

    if (verbose > 0)
    {
        printf(" Creating list of .rhosts files...\n");
    }


    /* generate list of .rhosts files */
    tempfile = "/tmp/lsat1.lsat";
    shellcode = "find / -mount -name .rhosts 2>/dev/null | tee >/tmp/lsat1.lsat";
    header = "This is a list of .rhosts files found on the system:\n";
    
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
	/* something went wrong */
	perror(" Creation of .rhosts list failed.");
	return(-1); 
    }


    if (verbose > 0)
    {
        printf(" Creating list of .netrc files...\n");
    }


    tempfile = "/tmp/lsat1.lsat";
    shellcode = "find / -mount -name .netrc 2>/dev/null | tee >/tmp/lsat1.lsat";
    header = "This is a list of .netrc files found on the system\n";
    
    /* get a list of .netrc files */
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
	/* something went wrong */
	perror(" Creation of .netrc list failed.");
	return(-1);
    }

    if (verbose > 0)
    {
	printf(" Creating list of dotfiles...\n");
    }

    tempfile = "/tmp/lsat1.lsat";
    shellcode = "find / -mount -name \" \" -print 2>/dev/null | tee >/tmp/lsat1.lsat";
    header = "This is a list of dotfiles found on the system\n";
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
	/*something went wrong */
	perror(" Creation of dot list failed.");
	return(-1);
    }
    

    if (verbose >0)
    {
        printf(" Finished in checkdotfiles module.\n");
    }
    return(0);
}
