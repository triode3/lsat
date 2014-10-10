/****************************************/
/* This is the checklimits.c module for */
/* lsat program. This module checks for */
/* the default setting in limits.conf   */
/* This is not a complete check by far, */
/* but a good place to start.		*/
/*					*/
/* See lsatmain.c for more details.	*/
/****************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int checklimits(filename, verbose, html)
const char *const filename;
int verbose;
int html;
{
    const char * header   =NULL;
    const char * tempfile =NULL;
    const char * shellcode =NULL;

    if (verbose >= 0)
    {
    	printf(" Running checklimits module...\n");
    }

    /* first check for the limits.conf file */
    /* in the usual place. if its not there */
    /* tell the user we cant find it.       */

    if ((system("test -r /etc/security/limits.conf")) == 0)
    {
	/* it exists, check it */
        /* -v inverts sense of return value */
        /* if == 1 there was a match (it is hashed out).    */
        /* if so, then we warn user.			*/
        if ((system("grep \"\\*\" /etc/security/limits.conf 2>/dev/null |grep -v \\# 2>/dev/null")) != 0)
        {
	    header = "Default limits hashed out in limits.conf.\nCheck /etc/security/limits.conf for the default entry.\nMake sure to set hard and soft limits for default \"*\",\nor for individual users.\n"; 
        }
        else
        {
	    header = "/etc/security/limits.conf looks ok.\nDouble check the limits just to be sure.\n";
        }
    }
    else
    {
	header = "Can't seem to find limits.conf file.\nCheck to make sure you are using limits and,\nthat the default (hard/soft) limits are set.\n";
    }
    
    /* ok, call dostuff to print out the header */
    /* to the output file... 			*/
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* something went wrong */
	perror(" Creation of list failed.");
	return (-1);
    }

    /* ok, we looked for the system limits in   */
    /* the limits file, but lets see what the   */
    /* current limits are...			*/
    /* we think this works on all distros. :O   */
    shellcode = "ulimit -a 2>/dev/null >>/tmp/lsat1.lsat";
    header = "Output from ulimit, check to see if these are reasonable limits.\nResource limits can help prevent DOS attacks,\nread up on them if you need to.\n";
    tempfile = "/tmp/lsat1.lsat";
    if ((dostuff(tempfile, filename, shellcode, header, html)) <0)
    {
	/* something went wrong */
	perror(" Creation of list failed.");
	return(-1);
    }
    
    if (verbose > 0)
    {
        printf(" Finished in checklimits module.\n");
    }

    return(0);
}
