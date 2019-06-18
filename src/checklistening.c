/**********************************************/
/* This will check for applications listening */ 
/* on the system using /proc/net/packet and   */
/* lsof because ifconfig and ip don't pick    */
/* up everything.                             */
/*                                            */
/* False positives like dhcpcd, dhclient and  */
/* wpa_supplicant are expected but it is      */
/* always "better" to know what =is= running. */
/*                                            */
/*                                            */ 
/* See lsatmain.c for more details.           */
/**********************************************/
#include "lsatheader.h" 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int checklistening(filename, distribution, verbose, html)
const char *const filename;
int distribution;
int verbose;
int html;
{
    const char * header   =NULL;
    const char * tempfile =NULL;
    const char * shellcode=NULL;
  
    int lsof_installed = 0;

    if (verbose >= 0)
    {
        printf(" Running checklistening module...\n");
    }

    /* it was noted that some distros do not install lsof by    */
    /* default, mainly gentoo... we therefore check here.       */
    if (distribution != 3)
    {
        if ((system("lsof 1>/dev/null 2>/dev/null >/dev/null")) != 0 ) 
        {
            /* perhaps lsof is not installed */
	    header = "lsof is not installed on this system,\nor it is not in the path,\nor I just can not find it.\ncheckopenfiles was not run.\n";
	    lsof_installed = 1;
        }
    }

    if ((system("readlink 1>/dev/null 2>/dev/null >/dev/null")) != 0 ) 
    {
        /* readlink aint there */
	    header = "readlink is not installed on this system,\nor it is not in the path,\nor I just can not find it.\nchecklistening was not run.\n";
        /* we'll just use the other var since it makes no differency because we need both */
	    lsof_installed = 1;
    }

    /* Solaris       */
    if (distribution == 3)
    {
        header = "checklistening not supported on Solaris.\n";
        lsof_installed = 1;
    } 

    if (lsof_installed == 0)
    {
    	header = "These applications are listening (processname, pid):\n";
    	tempfile = "/tmp/lsat1.lsat";
    	shellcode = "grep /proc/net/packet -ve ^sk|awk '{print $9}'|while read inode; do lsof -lMnP | grep '$inode' | awk '{print $2}'|while read pid; do name=$(readlink -f /proc/$pid/exe); echo '${name} $pid'; done 2>/dev/null >>/tmp/lsat1.lsat";
    	if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    	{
       		/* something went wrong */
        	perror(" Creation of list failed.");
        	return(-1);
    	}
    }

    if (lsof_installed == 1)
    {
	if ((dostuff(0, filename, 0, header, html)) < 0)
        {
                /* something went wrong */
                perror(" Creation of list failed.");
                return(-1);
        }
    }


    if (verbose > 0)
    {
        printf(" Finished in checklistening module.\n");
    }

    return(0);
}

