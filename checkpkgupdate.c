/****************************************/
/* This is the checkpkgupdate module for*/
/* the LSAT program. It cehcks to see if*/
/* any packages would be updated if the */
/* the user were to do a package update */
/****************************************/

#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int checkpkgupdate(filename, distribution, verbose, html)
const char *const filename;
int distribution;
int verbose;
int html;
{

    /* recall our distributions of note... this module	*/
    /* will currently only check redhat/centos, gentoo	*/
    /* debian and derivatives (ubuntu, mint, etc). 	*/
    /* I am currently not checking solaris or Mac OS X	*/
    const char * tempfile =NULL;
    const char * shellcode=NULL;
    const char * header   =NULL;

    if (verbose >= 0)
    {
        printf(" Running checkpkgupdate module...\n");
    }


    if (verbose > 0)
    {
	printf(" Checking for possible package updates...\n");
    }

    tempfile = "/tmp/lsat1.lsat";
    header = "List of packages that would be updated if the system packages were updated.\nPlease review this list.\nConsider running a package list update also.\n";

    /* check the distribution and perform the correct operation */
    /* lsatmain should ignore the other distributions for us    */
	/* NOTE, we are redirecting stderr to stdin as we want msgs */
	/* to be presented to the user 								*/
    if (distribution == 1)
    {
    	/* we are on redhat */
	shellcode = "yum check-update >/tmp/lsat1.lsat 2>&1";
    }
    if ((distribution == 2) || (distribution == 7) || (distribution == 8))
    {
        /* we are on debian, ubuntu or mint */
	shellcode = "apt-get -s dist-upgrade | awk '/^Inst/ { print $2 }' 2>&1 >/tmp/lsat1.lsat";
    }
    if (distribution == 4)
    {
    	/* we are on gentoo */
	shellcode = "emerge -pv --update @world >/tmp/lsat1.lsat 2>&1";
    }

   
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
        /* rhut-rho...something bad happened */
        perror(" Creation of list failed.");
        return(-1);
    }
    
    if (verbose > 0)
    {
        printf(" Finished in checkpkgupdate module.\n");
    }
   
    return(0);
}
