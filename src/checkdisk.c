/****************************************/
/* This is the checkdisk.c module for 	*/
/* lsat program. This module checks 	*/
/* disk space usage and mount points.   */
/*					*/
/* See lsatmain.c for more details.	*/
/****************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int checkdisk(filename, verbose, html)
const char *const filename;
int verbose;
int html;
{
    const char * tempfile =NULL;
    const char * shellcode=NULL;
    const char * header   =NULL;

    if (verbose >= 0)
    {
    	printf(" Running checkdisk module...\n");
    }

    if (verbose > 0)
    {
        printf(" Creating list of mount points...\n");
    }

    
    tempfile = "/tmp/lsat1.lsat";
    shellcode= "/bin/mount 2>/dev/null >/tmp/lsat1.lsat";
    header   = "This is a list of mount points currently mounted.\nMake sure the permissions are reasonable (rw, ro, etc).\n";
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
	/* something went wrong */
	perror(" Creation of list failed.");
	return (-1);
    }

    if (verbose > 0)
    {
        printf(" Creating list of disk usages...\n");
    }

    tempfile = "/tmp/lsat1.lsat";
    shellcode= "/bin/df -k 2>/dev/null >/tmp/lsat1.lsat";
    header   = "This is a list of disk utilizations on the system, in kilobytes.\nChcek to see that filesystems are not near capacity, etc.\n";
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
        /* something went wrong */
	perror(" Creation of list failed.");
	return (-1);
    }

    if (verbose > 0)
    {
        printf(" Finished in checkdisk module.\n");
    }

    return(0);
}
