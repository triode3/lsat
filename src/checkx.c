/****************************************/
/* This is the checkx module for LSAT   */
/* It will check for the nolisten option*/
/* in the X startup files               */
/*					*/
/* See lsatmain.c for more details.	*/
/****************************************/
/*					*/
/* note: the user could put some command*/
/* options to startx on the command line*/
/* and therefore, we should check the   */
/* ps output as well as xinitd and      */
/* other places to make sure we at least*/
/* check most configs...                */
/****************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int checkx(filename, verbose, html)
const char *const filename;
int verbose;
int html;
{
    const char * header   =NULL;
    const char * tempfile =NULL;
    const char * shellcode=NULL;
    int listen; /* are we listening? :) */
    listen = 1;

    if (verbose >= 0)
    {
    	printf(" Running checkx module...\n");
    }

    /* Check for X startup locations  */
    /* don't forget the user could be doing gdm or xdm */
    if ((system("test -r /usr/X11R6/bin/startx")) == 0)
    {
	/* it exists, check it */
        if ((system("grep -i defaultserverargs /usr/X11R6/bin/startx 2>/dev/null |grep \"nolisten tcp\" 1>/dev/null 2>/dev/null")) == 0)
        {
            listen = 0;
	    header = "X is not listening for tcp, good.\n"; 
        }
    }
    /* check gdm... */
    if ((system("test -r /etc/X11/gdm/gdm.conf")) == 0)
    {
        if ((system("grep -i \"command=/usr/X11R6\" /etc/X11/gdm/gdm.conf 2>/dev/null |grep \"nolisten tcp\" 1>/dev/null 2>/dev/null")) == 0)
        {
            listen = 0;
            header = "X is not listening for tcp, good.\n";
        }
    }
    /* check xdm and kdm... */
    if ((system("test -r /etc/X11/xdm/Xservers")) == 0)
    {
        if ((system("grep -i \"command=/usr/X11R6\" /etc/X11/xdm/Xservers 2>/dev/null |grep \"nolisten tcp\" 1>/dev/null 2>/dev/null")) == 0)
        {
            listen = 0;
            header = "X is not listening for tcp, good.\n";
        }
    }
    /* check fs... */
    if ((system("test -r /etc/X11/fs/config")) == 0)
    {
        if ((system("grep -qi \"^no-listen = tcp\" /etc/X11/fs/config 1>/dev/null 2>/dev/null")) == 0)
        {
            listen = 0;
            header = "X is not listening for tcp, good.\n";
        }
    }
    if (listen == 1)
    {
        /* found xconfig, but no -nolisten tcp */
        header = "X seems to be listening for tcp connections.\nConsider turning this off with\n-nolisten tcp in your X startup file.\n";
    }
    
    /* ok, call dostuff to print out the header */
    /* to the output file... 			*/
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* something went wrong */
	perror(" Creation of list failed.");
	return (-1);
    }

    /* ok, check and see if startx is running,  */
    /* and print that out... this way we catch  */
    /* if the user started it from the CLI with */
    /* some listen options.                     */
    if ((system("ps -afl |grep startx 2>/dev/null |grep -v grep 1>/dev/null 2>/dev/null") == 0))
    {
        /* we have found a startx in ps listing, print it out   */
        shellcode = "ps -afl |grep startx 2>/dev/null |grep -v grep 2>/dev/null >/tmp/lsat1.lsat";
        header = "This is the actual startx command, with info.\nIf it contains a listen or vty or pty, etc,\nconsider changing command line options.\n";
        tempfile = "/tmp/lsat1.lsat";
        if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
        {
            /* something went wrong */
            perror(" Creation of list failed.");
            return (-1);
        } 
    }

    if (verbose > 0)
    {
        printf(" Finished in checkx module.\n");
    }

    return(0);
}
