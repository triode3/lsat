/****************************************/
/* This is the checkinittab module for  */
/* LSAT. It merely checks to see which  */
/* runlevel is running, or actually, it */
/* just checks to see if we are running */
/* in 5 (Xwindows)... note this is not  */
/* for Mac OSX. 			*/
/*					*/
/* See lsatmain.c for more details.	*/
/****************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int checkinittab(filename, distribution, verbose, html)
const char *const filename;
int distribution;
int verbose;
int html;
{
    const char * header = NULL;

    if (verbose >= 0)
    {
    	printf(" Running checkinittab module...\n");
    }

    /* if we are not on redhat or derivative, do not do this... */
    if (distribution != 5)
    {
    	/* we are not on Mac OSX */
        if ((system("grep id:5:initdefault /etc/inittab 1>/dev/null 2>/dev/null >/dev/null") == 0))
	{
		/* we have a match, they are running X as default init */
		header = "default init level set to 5.\nUnless there is a reason to run a GUI full time on this system,\nconsider setting to runlevel 3.\n";
	}
	else
	{
		/* we (hope) the runlevel is 3 and not 6 *smile* */
		header = "default init level is not set to 5. Good.\n";
 	}
    }

    /* slackware check */
    if (distribution == 6) /* JTO */
    {
        /* we are on slackware */
        if ((system("grep id:4:initdefault /etc/inittab 1>/dev/null 2>/dev/null >/dev/null")) == 0)
        {
            /* we have a match, they are running X as default init */
            header = "default init level set to 4.\nUnless there is a reason to run a GUI full time on this system,\nconsider setting to runlevel 3.\n";
        }
        else
        {
            /* we (hope) the runlevel is 3 and not 6 *smile* */
            header = "default init level is not set to 4. Good.\n";
        }
    }
	
    /* ok, call dostuff to print out the header */
    /* to the output file... 			*/
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* something went wrong */
	perror(" Creation of list failed.");
	return (-1);
    }
    
    if (verbose > 0)
    {
        printf(" Finished in checkinittab module.\n");
    }

    return(0);
}
