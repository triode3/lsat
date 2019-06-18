/****************************************/
/* This is the checcfg.c module for 	*/
/* lsat program. This module checks 	*/
/* all services in all runlevels on the */ 
/* system and just prints them out...   */
/* this is redhat specific...		*/
/*					*/
/* See lsatmain.c for more details.	*/
/****************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int checkcfg(filename, verbose, html)
const char *const filename;
int verbose;
int html;
{
    const char * tempfile =NULL;
    const char * shellcode=NULL;
    const char * header   =NULL;

    if (verbose >= 0)
    {
    	printf(" Running checkcfg module...\n");
    }

    if (verbose > 0)
    {
        printf(" Creating list of daemons in all runlevels...\n");
    }

    
    tempfile = "/tmp/lsat1.lsat";
    shellcode= "/sbin/chkconfig --list 2>/dev/null | tee >/tmp/lsat1.lsat";
    header   = "This is a list of all services in\n all runlevels on the system.\n";
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
	/* something went wrong */
	perror(" Creation of list failed.");
	return (-1);
    }

    tempfile = "/tmp/lsat1.lsat";
    shellcode = "/sbin/chkconfig --list 2>/dev/null |grep on >/tmp/lsat1.lsat";
    header = "This is a list of all services which are \non right now, taken from chkconfig output.\n";
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
	/* something went wrong */
	perror( "Creation of list failed.");
	return(-1);
    }

    if (verbose > 0)
    {
        printf(" Finished in checkcfg module.\n");
    }

    return(0);
}
