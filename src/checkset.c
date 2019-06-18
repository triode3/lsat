/****************************************/
/* This is the checkset.c module for	*/
/* lsat program. This module checks for	*/
/* setuid and setgid files/dirs on the	*/
/* local system and prints them to the	*/
/* outfile.				*/
/*					*/
/* 4/5/2002, added code to check for	*/
/* files in /dev that do not belong	*/
/*					*/
/* See lsatmain.c for more details.	*/
/****************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int checkset(filename, verbose, html)
const char *const filename;
int verbose;
int html;
{

    const char *const tempfile="/tmp/lsat1.lsat";
    const char * shellcode;
    const char * header;


    /* Generate a list of SUID/SGID files/dirs. 	*/
    /* This should be done from /, but use -xdev so	*/
    /* that we don't traverse other filesystems.	*/
    /* Note, we use -mount for compatibility with 	*/
    /* other versions of find.			*/

    if (verbose >= 0)
    {
        printf(" Running checkset module...\n");
    }

    if (verbose > 0)
    {
        printf(" Creating list of SUID files...\n");
        printf(" Note: we are not traversing \"other\" filesystems.\n");

    }

    shellcode="find / -mount -perm +4000 2>/dev/null | tee >/tmp/lsat1.lsat";
    header   ="This is a list of SUID files on the system:\n";
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
	/* something went wrong */
	perror(" Error in module checkset. Aborting.");
	return (-1);
    }

    if (verbose > 0)
    {
        printf(" Creating list of SGID files and directories...\n");
        printf(" Note: we are not traversing \"other\" filesystems.\n");
    }

    shellcode="find / -mount -perm -2000 2>/dev/null | tee >/tmp/lsat1.lsat";
    header   ="This is a list of SGID files/directories on the system:\n";
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
	/* something went wrong */
	perror(" Error in module checkset. Aborting");
	return (-1);
    }



    if (verbose > 0)
    {
        printf(" Creating list of normal files in /dev...\n");
    }

/* Pass by /dev/{watchdog,MAKEDEV.ibcs,X0R}  */

    shellcode="find /dev -mount -type f 2>/dev/null|grep -v X0R 2>/dev/null|grep -v watchdog 2>/dev/null|grep -v MAKEDEV.ibcs 2>/dev/null| tee > /tmp/lsat1.lsat";
    header   ="List of normal files in /dev. MAKEDEV is ok, but there\nshould be no other files:\n";
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
	/* something went wrong */
	perror(" Error in module checkset. Aborting");
	return(-1);
    }


    if (verbose > 0)
    {
        printf(" Finished in checkset module.\n");
    }

    return(0);
}
