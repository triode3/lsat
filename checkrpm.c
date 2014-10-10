/****************************************/
/* This is the checkrpm module for the 	*/
/* LSAT program. It performs a rpm -Va  */
/* on the system. Please read the 	    */
/* README.checkrpm file for details.    */
/****************************************/

#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int checkrpm(filename, verbose, html)
const char *const filename;
int verbose;
int html;
{

    /* note if distribution = 1, redhat */
    /* if distribution = 2, debian */

    const char * tempfile =NULL;
    const char * shellcode=NULL;
    const char * header   =NULL;

    if (verbose >= 0)
    {
        printf(" Running checkrpm module...\n");
    }


    if (verbose > 0)
    {
	printf(" Checking rpm integrity...\n");
    }

    tempfile = "/tmp/lsat1.lsat";
    header = "Integrity check of rpm packages.\nPlease read README.redhat for more info.\n";
    shellcode = "rpm -Va 2>/dev/null >/tmp/lsat1.lsat";
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
        /* rhut-rho...something bad happened */
        perror(" Creation of list failed.");
        return(-1);
    }
    
    if (verbose > 0)
    {
        printf(" Finished in checkrpm module.\n");
    }
   
    return(0);
}
