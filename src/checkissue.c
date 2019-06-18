/********************************************/
/* this module checks for existance of      */
/* /etc/issue and /etc/issue.net files...   */
/* If they exist, this is not really bad,   */
/* but by default they give out system      */
/* specific information which could be      */
/* useful to an attacker. 		    */
/*                                          */ 
/* See lsatmain.c for more details.         */
/********************************************/
#include "lsatheader.h" 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int checkissue(filename, verbose, html)
const char *const filename;
int verbose;
int html;
{
    const char * header   =NULL;
  
    if (verbose >= 0)
    {
        printf(" Running checkissue module...\n");
    }

    if ((system("cat /etc/issue 1>/dev/null 2>/dev/null >/dev/null")) == 0)
    /* its there, This is a "bad thing", perhaps. */
    {
	header = "/etc/issue exists. Make sure it does not have any \n system specific information in it.\n";
        if ((dostuff(0, filename, 0, header, html)) < 0)
        {
                /* something went wrong */
                perror(" Creation of list failed.");
                return(-1);
        }
    } 
    else /* /etc/issue does not exist */
    {
        header = "/etc/issue does not exist. Good.\n";
        if ((dostuff(0, filename, 0, header, html)) < 0)
        {
                /* something went wrong */
                perror(" Creation of list failed.");
                return(-1);
        }
    }


    if ((system("cat /etc/issue.net 1>/dev/null 2>/dev/null >/dev/null")) == 0)
    /* its there, This is a "bad thing", perhaps. */
    {
	header = "/etc/issue.net exists. Make sure it does not have any \n system specific information in it.\n";
        if ((dostuff(0, filename, 0, header, html)) < 0)
        {
                /* something went wrong */
                perror(" Creation of list failed.");
                return(-1);
        }
    }
    else /* /etc/issue.net does not exist */
    {
        header = "/etc/issue.net does not exist. Good.\n";
        if ((dostuff(0, filename, 0, header, html)) < 0)
        {
                /* something went wrong */
                perror(" Creation of list failed.");
                return(-1);
        }
    }

     if ((system("cat /etc/motd 1>/dev/null 2>/dev/null >/dev/null")) == 0)
    /* its there, This is a "bad thing", perhaps. */
    {
        header = "/etc/motd exists. Make sure it does not have any \n system specific information in it.\n";
        if ((dostuff(0, filename, 0, header, html)) < 0)
        {
                /* something went wrong */
                perror(" Creation of list failed.");
                return(-1);
        }
    }
    else /* /etc/motd does not exist */
    {
        header = "/etc/motd does not exist.\n";
        if ((dostuff(0, filename, 0, header, html)) < 0)
        {
                /* something went wrong */
                perror(" Creation of list failed.");
                return(-1);
        }
    }

    if ((system("ls /etc/banners 1>/dev/null 2>/dev/null >/dev/null")) == 0)
    {
	/* it should be there then */
        header = "/etc/banners exists... Check it to make sure its ok.\n";
    }
    else
    {
        /* it is not there... */
        header = "/etc/banners dir not found. \nCheck securitylinks.txt for more info.\n";
    }
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.");
        return(-1);
    }


    if (verbose > 0)
    {
        printf(" Finished in checkissue module.\n");
    }

    return(0);
}

