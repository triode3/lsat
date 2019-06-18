/****************************************/
/* This is the cheklogging module for   */
/* LSAT. It merely checks that log auth */
/* and log authpriv facilities are being*/
/* used. 				*/
/* It now also checks the faillog.      */
/* We will also check lastlog here.     */
/*					*/
/* See lsatmain.c for more details.	*/
/****************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int checklogging(filename, distribution, verbose, html)
const char *const filename;
int distribution;
int verbose;
int html;
{
    const char * header = NULL;
    const char * shellcode = NULL;
    const char * tmp_file = "/tmp/lsat1.lsat";

    if (verbose >= 0)
    {
        printf(" Running checklogging module...\n");
    }

    /* if we are not on redhat or derivative, do not do this... */
    /* we shall append to this later for Solaris and Mac OSX    */

    /* check for the auth facility... */
    if (distribution != 5)
    {
    	/* we are not on Mac OSX */
        if ((system("grep auth\\. /etc/syslog.conf  1>/dev/null 2>/dev/null >/dev/null") == 0))
	{
	    /* match, they are logging auth */
	    header = "_Looks_ like you are using the auth log facility\nin syslog. Good.";
	}
	else
	{
            /* no match. they are not logging auth */
	    header = "Consider placing:\n\n auth.*\t\t\t\t/var/log/secure\n\n in your /etc/syslog.conf file.\n";
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

    /* now check for authpriv facility... */
    if (distribution != 5)
    {
        /* we are not on Mac OSX */
        if ((system("grep authpriv\\. /etc/syslog.conf  1>/dev/null 2>/dev/null >/dev/null") == 0))
        {
            /* match, they are logging authpriv */
            header = "_Looks_ like you are using the authpriv log facility\nin syslog. Good.";
        }
        else
        {
            /* no match. they are not logging authpriv */
            header = "Consider placing:\n\n authpriv.*\t\t\t\t/var/log/secure\n\n in your /etc/syslog.conf file.\n";
        }
    }
    
    /* ok, call dostuff to print out the header */
    /* to the output file...                    */
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.");
        return (-1);
    }

    /* Solaris and Linux should have this... I think OSX has it too */
    /* note we have to check for /var/log/faillog as a number of    */
    /* modern OS's are dropping faillog, so the binary exisrts, but */
    /* it is not running 					    */
    if (((system("test -r /usr/bin/faillog")) == 0) && (system("test -r /var/log/faillog") == 0)) 
    {
        /* run failllog */
	/* ops, we have to shorten the output of faillog */
	/* e.g. what if we are on a big system w/1000s of*/
	/* users? The faillog output could be huge...    */
	/* we cut it back to say 100...			 */
	shellcode = "/usr/bin/faillog -a |head -100 2>/dev/null >/tmp/lsat1.lsat";
	header = "The last 100 (or less) failed login attempts on the system\n";
        if ((dostuff(tmp_file, filename, shellcode, header, html)) < 0)
	{
	    /* something went wrong */
	    perror(" Creation of list failed.");
	    return(-1);
	}
    }

    /* Check lastlog here and give the user the output.  */
    /* Note, I am removing "never logged in" entries     */
    /* this note was incorrect, it should have run grep  */
    /* Also, last -d -F -w seems to be a much better way */
    /* to go to get more info, but it is long if you have*/
    /* a lot of users. Might have to see about changing  */
    /* that in the future...                             */
    /* So far, I see lastlog is always in /usr/bin.      */
    if ((system("test -r /usr/bin/last")) == 0)
    {
        /* run lastlog */
	shellcode = "/usr/bin/last -d -F -w 2>/dev/null >/tmp/lsat1.lsat";
	header = "This is the list of users who have logged into the system\n This includes reboots of the system.\n Username  tty  IPaddress  login  logout  duration\n";
	if ((dostuff(tmp_file, filename, shellcode, header, html)) < 0)
	{
	    /* something went wrong */
	    perror(" Creation of list failed.");
	    return(-1);
	}
    }

    if (verbose > 0)
    {
        printf(" Finished in checklogging module.\n");
    }

    return(0);
}
