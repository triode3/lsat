/****************************************/
/* This is the checkpasstime module for */
/* the LSAT program. It cehcks to see   */
/* when passwords are set to expire for */
/* user accounts on the system          */
/****************************************/

#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>

int checkpasstime(filename, distribution, verbose, html)
const char *const filename;
int distribution;
int verbose;
int html;
{

    /* I am currently not checking solaris or Mac OS X	*/
    const char * tempfile =NULL;
    const char * shellcode=NULL;
    const char * header   =NULL;

    if (verbose >= 0)
    {
        printf(" Running checkpasstime module...\n");
    }


    if (verbose > 0)
    {
	printf(" Checking for users password expiration dates...\n");
    }

    tempfile = "/tmp/lsat1.lsat";
    header = "List of users on the system with UID over 1000\nand the associated password expiration information\n";
	shellcode = "for x in `cat /etc/passwd |awk -F \":\" '{ if ( $3 > 999 && $3 < 65000) print $1 }'`; do echo \"User:\" $x; chage -l $x; done >/tmp/lsat1.lsat";

    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
        /* rhut-rho...something bad happened */
        perror(" Creation of list failed.");
        return(-1);
    }
    
    if (verbose > 0)
    {
        printf(" Finished in checkpasstime module.\n");
    }
   
    return(0);
}
