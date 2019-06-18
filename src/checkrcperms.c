/*****************************************/
/* This is the checkrcperms.c module for */
/* lsat program. This module checks for  */
/* files in /etc/rc.d/init.d without the */
/* permission 700.                       */
/* This module by Jose Morelli Neto,     */
/* with added support for Solaris, gentoo*/
/* by Triode...				 */
/*                                       */
/* See lsatmain.c for more details.      */
/*****************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int checkrcperms(filename, distribution, verbose, html)
const char *const filename;
int distribution;
int verbose;
int html;
{

    const char *const tempfile="/tmp/lsat1.lsat";
    const char * shellcode;
    const char * header;


    /* Generate a list of files in /etc/rc.d/init.d     */
    /* That have permission equal 700                   */

    if (verbose >= 0)
    {
        printf(" Running checkrcperms module...\n");
    }

    if (verbose > 0)
    {
        printf(" Checking files in init.d for perms...\n");
    }

    if (distribution == 1) 
    {
		/* redhat/centos */
        shellcode="find /etc/rc.d/init.d/ -type f ! \\( -perm 700 \\) -exec ls  {} \\; | tee >/tmp/lsat1.lsat";
        header   ="This is a list of files in /etc/rc.d/init.d whose permissions are not set to 700.\nWe recommend that you change the permission of these files to 700.\n";
        if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
        {
            /* something went wrong */
            perror(" Error in module checkrcperms. Aborting.");
            return (-1);
        }
    } 

	if ((distribution == 2) || (distribution == 7) || (distribution == 8))
	{
		/* debian/ubuntu/mint */
		shellcode="find /etc/init.d/ -type f ! \\( -perm 700 \\) -exec ls {} \\; |tee >/tmp/lsat1.lsat";
		header = "This is a list of files in /etc/init.d whose permissions are not set to 700.\nWe recommend that you change the permissions of these files to 700.\n";
		if ((dostuff(tempfile, filename, shellcode, header, html)) <0)
		{
			/* something went wrong */
			perror(" Error in module checkrcperms, Aborting.");
			return (-1);
		}
	}

    if ((distribution == 3) || (distribution == 4))
    {
		/* we are on Solaris or gentoo  */
        shellcode="find /etc/init.d/ -type f ! \\( -perm 700 \\) -exec ls {} \\; | tee >/tmp/lsat1.lsat";
        header = "This is a list of files in /etc/init.d whose permissions are not set to 700.\nWe recommend that you change the permissions of these files to 700.\n";
        if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    	{
            /* something went wrong */
            perror(" Error in module checkrcperms. Aborting.");
            return (-1);
        }
    }

    if ((distribution == 5))
    {
        /* we are on Mac OS X  */
	; /* smile */
    }

    if ((distribution == 6)) /* JTO */
    {
        /* slack */
        ; /* there is no init.d, we will check later on for states */
    }

    if (verbose > 0)
    {
        printf(" Finished in checkrcperms module.\n");
    }

    return(0);
}
