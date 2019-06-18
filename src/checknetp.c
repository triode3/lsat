/****************************************/
/* This is the checknetpromis module for*/
/* lsat. It checks to see if the ethX   */
/* is in promisuous mode...   		*/
/*					*/
/* See lsatmain.c for calling function. */
/****************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int checknetp(kernel, filename, distribution, verbose, html)
char kernel[];
const char *const filename;
int distribution;
int verbose;
int html;
{

    const char * header   =NULL;
    const char * tempfile =NULL;
    char kernver[3][4]={"2.2", "2.4", "2.6"};

    if (verbose >= 0)
    {
        printf(" Running checknetpromisc module...\n");
    }


    if (verbose > 0)
    {
	printf(" Checking for promiscuous mode...\n");
    }

    if ((distribution != 3) && (distribution != 5))
    {
	/* we are on linux... */
	/* note we need to check how to do this on */
	/* solaris and the like so it should not   */
	/* be run at all from lsatmain... 	   */
	/* ... therefore the above check is redundant */

	/* if we are on a 2.6 or greater kernel,   */
	/* this does not work. Check that first... */
   	if ((strcmp(kernel,kernver[2]))== 0)
	{
	    /*we are on 2.6, uhoh */
		header = "You are on a 2.6 kernel, you must check syslog\n to see if you are in promisc mode\n";
	}
	else
	{ /* we are on something else */

        if ((system("test -x /sbin/ip")) == 0)
	{
            if ((system("/sbin/ip link show|grep -e PROMISC|cut -d':' -f 2 2>/dev/null >> /tmp/lsat1.lsat")) == 0)
            {
                header = "These network interfaces found to be in promisc mode using /sbin/ip.\n";
	    }
	    else
	    {
	        header = "No interfaces found in promiscuous mode. Good.\n";
	    }
        }
        else
        {
	    /* note gentoo has moved ipconfig depending on version so we need to check first */
	    if ((system("test -r /sbin/ifconfig")) == 0)
	    {
                if ((system("/sbin/ifconfig |grep -B 2 PROMISC 2>/dev/null |grep Link 2>/dev/null |awk -F" " 'length($1) > 0 {print $1}' 2>/dev/null >> /tmp/lsat1.lsat")) == 0)
                {
                    header = "No interfaces found in promisc mode. Good.\n";
                }
	    }
	    if ((system("test -r /bin/ifconfig")) == 0)
	    {
	        if ((system("/bin/ifconfig |grep -B 2 PROMISC 2>/dev/null |grep Link 2>/dev/null |awk -F" " 'length($1) > 0 {print $1}' 2>/dev/null >> /tmp/lsat1.lsat")) == 0)
		{
		    header = "No interfaces found in promisc mode. Good.\n";
		}
	    }
        }
        tempfile = "/tmp/lsat1.lsat";
        if ((dostuff(tempfile, filename, 0, header, html)) < 0)
        {
            /* rhut-rho...something bad happened */
            perror(" Creation of list failed.");
            return(-1);
        }

	} /* end of else...*/
        /* now we write out our heder with no tempfile */	
	if ((dostuff(0, filename, 0, header, html)) <0)
	{
	    perror("Creation of list failed.");
	    return(-1);
	}
    }
    
    if (verbose > 0)
    {
        printf(" Finished in checkpromisc module.\n");
    }
   
    return(0);
}
