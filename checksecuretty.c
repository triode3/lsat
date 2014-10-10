/********************************************/
/* this module does a (simple) check to see */
/* if ttys above 6 are on the system.       */
/* In general, its a bad idea to have above */
/* 6, since root can log into them. root    */
/* should only have console access...       */
/* so also check for anything other than    */
/* "tty" in the file...			    */
/*                                          */ 
/* See lsatmain.c for more details.         */
/********************************************/
#include "lsatheader.h" 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int checksecuretty(filename, verbose, html)
const char *const filename;
int verbose;
int html;
{
    const char * header   =NULL;
  
    if (verbose >= 0)
    {
        printf(" Running checksecuretty module...\n");
    }

    if ((system("grep tty7 /etc/securetty 1>/dev/null 2>/dev/null >/dev/null")) == 0)
    /* its in there. Keep in mind this is a simple check... */
    {
	header = "/etc/securetty has tty's over 6.\nConsider disabeling all ttys over tty6 (console).\n";
        if ((dostuff(0, filename, 0, header, html)) < 0)
        {
                /* something went wrong */
                perror(" Creation of list failed.");
                return(-1);
        }
    }

    if ((system("grep -v tty /etc/securetty 1>/dev/null 2>/dev/null >/dev/null")) ==0 )
    /* some console other than tty is there, probably vty/n */
    {
        header = "/etc/securetty has ttys other than the console.\nConsider removing any lines in /etc/securetty other than tty[1-6].\n";
        if ((dostuff(0, filename, 0, header, html)) < 0)
        {
                /* something went wrong */
                perror(" Creation of list failed.");
                return(-1);
        }
    }
 

    if (verbose > 0)
    {
        printf(" Finished in checksecuretty module.\n");
    }

    return(0);
}

