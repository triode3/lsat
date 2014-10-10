/****************************************/
/* This is the checkftp module for lsat */
/* It will check common ftp servers cfgs*/
/* in the usual locations, for errors...*/
/*                                      */
/****************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int checkftp(filename, distribution, verbose, html)
const char *const filename;
int distribution;
int verbose;
int html;
{
    const char * header   =NULL;

    if (verbose >= 0)
    {
        printf(" Running checkftp module...\n");
    }

    if (verbose > 0)
    {
	printf(" Checking ftp configs...\n");
    }

    /* note if distribution = 4, we are on gentoo... */

    /* check pure-ftpd on gentoo */
    /* note, pure-ftpd compiled by hand typically has*/
    /* no "real" config file, you just do cmd line   */
    /* switches... so we really don't check that yet */
    if (distribution == 4)
    {
    /* we are on gentoo... */
        if ((system("test -r /etc/conf.d/pure-ftpd")) == 0)
        {
            if ((system("grep \"AUTH=-lpuredb:\" /etc/conf.d/pure-ftpd")) == 0)
            {
                /* found it, they have the auth file */
                header = "Found auth file in pro-ftpd config, good.\n";
            }
            else
            {
                /* they have the conf file, but no auth line */
                header = "Did not find AUTH line in pro-ftpd conf file.\nCheck the manpage for more info and add one\n.";
            }
        }
        if ((dostuff(0, filename, 0, header, html)) < 0)
        {
            /* rhut-rho...something bad happened */
            perror(" Creation of list failed.");
            return(-1);
        }
    }

    /* check proftpd on gentoo */
    /* other distros typical install location of cfg file */
    /* is /usr/local/etc/proftpd.conf or /etc/proftpd.conf */
    if (distribution == 4)
    {
        /* we are on gentoo */
	/* first test to see if the proftpd is even there... */
	if ((system("test -r /etc/proftpd/proftpd.conf")) == 0)
	{
            if ((system("grep \"ServerIdent on\" /etc/proftpd/proftpd.conf 2>/dev/null >/dev/null")) == 0)
            {      
                /* found it */
                header = "Looks like proftpd is checking Ident, good.\n";
            }
            else
            {
                /* not there */
                header = "Looks like you have proftpd, but it is not checking Ident.\n";
            }
	}
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* rhut-rho...something bad happened */
        perror(" Creation of list failed.");
        return(-1);
    }
    }

    /* check some more proftpd on gentoo */
    if (distribution == 4)
    {
        /* we are on gentoo */
	/* again, test to see if it is there first */
	if ((system("test -r /etc/proftpd/proftpd.conf")) == 0)
	{
            if ((system("grep \"RequireValidShell off\" /etc/proftpd/proftpd.conf 2>/dev/null >/dev/null")) == 0)
            {
                /* found it */
                header = "Looks like proftpd is setup for virtual users.\nThis is good, double check it, though.\n";
            }
            else
            {
                header = "Looks like proftpd requires a valid shell.\nThis is bad. Set RequireValidShell to off and then use Virtual Users.\n";
            }
	}
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* rhut-rho...something bad happened */
        perror(" Creation of list failed.");
        return(-1);
    }
    }

    /* check some more proftpd on gentoo */
    if (distribution == 4)
    {
        /* we are on gentoo */
	/* again, test to see if it exists */
	if ((system("test -r /etc/proftpd/proftpd.conf")) == 0)
	{
        if ((system("grep \"Umask 077\" /etc/proftpd/proftpd.conf 2>/dev/null >/dev/null")) == 0)
        {
            /* found it */
            header = "Looks like proftpd is set Umask 077. Good.\n";
        }
        else 
        {
            header = "Looks like proftpd is not creating files umask 077.\nThis is bad. Check proftpd config and change it.\n";
        }
	}
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* rhut-rho...something bad happened */
        perror(" Creation of list failed.");
        return(-1);
    }
    }


    /* check more proftpd stuffs */
    if (distribution != 4)
    {
        /* we are not on gentoo */
	if ((system("test -r /etc/proftpd.conf")) == 0)
        {
	    if ((system("grep \"ServerIdent on\" /etc/proftpd.conf 2>/dev/null >/dev/null")) == 0)
            {
                /* found it */
                header = "Looks like proftpd is checking Ident, good.\n";
            }
	    else
	    {
	         header = "Looks like you have proftpd, but it is not checking Ident. \nOr I could not find the config file.\n";
	    }
 	}
	if ((system("test -r /usr/local/etc/proftpd.conf")) == 0)
	{
	    if ((system("grep \"ServerIdent on\" /etc/proftpd.conf 2>/dev/null >/dev/null")) == 0)
            {
                /* found it */
                header = "Looks like proftpd is checking Ident, good.\n";
            }
	    else
	    {
		 header = "Looks like you have proftpd, but it is not checking Ident. \nOr I could not find the config file.\n";
	    }
	}
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* rhut-rho...something bad happened */
        perror(" Creation of list failed.");
        return(-1);
    }
    }

     /* check some more proftpd  */
    if (distribution != 4)
    {
        /* we are not on gentoo */
	if ((system("test -r /etc/proftpd.conf")) == 0)
 	{       
	    if ((system("grep \"RequireValidShell off\" /etc/proftpd.conf 2>/dev/null >/dev/null")) == 0)
            {
                /* found it */
                header = "Looks like proftpd is setup for virtual users.\nThis is good, double check it, though.\n";
            }
	    else
	    {
		header = "Looks like proftpd requires a valid shell.\nThis is bad. Set RequireValidShell to off and then use Virtual Users.\n";
	    }
	}
	if ((system("test -r /usr/local/etc/proftpd.conf")) == 0)
	{
	    if ((system("grep \"RequireValidShell off\" /usr/local/etc/proftpd.conf 2>/dev/null >/dev/null")) == 0)
	    {
		/* found it */
		header = "Looks like proftpd is setup for virtual users.\nThis is good, double check it, though.\n";
	    }
	    else
	    {
		header = "Looks like proftpd requires a valid shell.\nThis is bad. Set RequireValidShell to off and then use Virtual Users.\n";
	    }
	}
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* rhut-rho...something bad happened */
        perror(" Creation of list failed.");
        return(-1);
    }
    }

     /* check some more proftpd  */
    if (distribution != 4)
    {
        /* we are not on gentoo */
 	if ((system("test -r /etc/proftpd.conf")) == 0)       
	{
	    if ((system("grep \"Umask 077\" /etc/proftpd.conf 2>/dev/null >/dev/null")) == 0)
            {
                /* found it */
                header = "Looks like proftpd is set Umask 077. Good.\n";
            }
	    else
	    {
		header = "Looks like proftpd is not creating files umask 077.\nThis is bad. Check proftpd config and change it.\n";
	    }
	}
	if ((system("test -r /usr/local/etc/proftpd.conf")) == 0)
	{
	    if ((system("grep \"Umask 077\" /usr/local/etc/proftpd.conf 2>/dev/null >/dev/null")) == 0)
            {
                /* found it */
                header = "Looks like proftpd is set Umask 077. Good.\n";
            }
            else     
            {
            header = "Looks like proftpd is not creating files umask 077.\nThis is bad. Check proftpd config and change it.\n";
            }
        }
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* rhut-rho...something bad happened */
        perror(" Creation of list failed.");
        return(-1);
    }
    }

    if (verbose > 0)
    {
        printf(" Finished in checkftp module.\n");
    }
    return(0);
}
