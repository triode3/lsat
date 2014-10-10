/************************************************/
/* This is the checkservices module for the 	*/
/* LSAT program. It checks which services are   */
/* started at boot time and displays that for   */
/* the user in the final output                 */
/************************************************/

#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int checkservices(filename, distribution, verbose, html)
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
        printf(" Running checkservices module...\n");
    }


    if (verbose > 0)
    {
	printf(" Listing services that start at boot...\n");
    }

	if (distribution == 1)
	{
		/* we are on redhat/centos/derivatives */
    	tempfile = "/tmp/lsat1.lsat";
    	header = "Checking services that start at boot.\n";
    	shellcode = "Run=$(grep id: /etc/inittab |awk -F: \'{ print $2 }\') ; chkconfig --list |grep \'$Run:pn\' 2>/dev/null >/tmp/lsat1.lsat";
	}

 	if (distribution == 2)
	{
		/* we are on Debian */
		tempfile = "/tmp/lsat1.lsat";
		header = "Checking services that start at boot.\n";
		/* This code reused from a comment post by nixCraft from cybercit.biz/faq/inux-determine-which-services-are-enabled-at-boot/  */
		shellcode = "Runs=$(runlevel | awk '{ print $2}') ; for service in /etc/rc${Runs}.d/*; do basename $service |grep '^S' | sed 's/S[0-9].//g' ;done 2>/dev/null >/tmp/lsat1.lsat";
	}
	

	if (distribution == 3)
	{
		/* we are on Solaris */
		/* NOTE: this is going to show all services and show the state */
		tempfile = "/tmp/lsat1.lsat";
		header = "Listing all system services and the state of each.\n";
		shellcode = "svcs -a 2>/dev/null >/tmp/lsat1.lsat";
	}

	if (distribution == 4)
	{
		/* we are on gentoo */
		tempfile = "/tmp/lsat1.lsat";
		header = "Checking services that start at boot.\n";
		shellcode = "rc-update show boot default 2>/dev/null >/tmp/lsat1.lsat";
	}

	if (distribution == 5)
	{
		/* we are on Mac OS X */
		/* this is not tested. there is probably a better way. 				*/
		/* we are not sure if this is even correct. I think I read: 		*/
		/* items in /Library/LaunchDaemons & /System/Library/LaunchDaemons  */
		/* start when the mac boots, and run as root.						*/
		/* items in /Library/LaunchAgents and /System/Library/LaunchAgents  */
		/* start when any user logs in and run as that user.				*/

		tempfile = "/tmp/lsat1.lsat";
		header = "Checking files that launch at boot or login.\n";
		shellcode = "ls /Library/LaunchDaemons /System/Library/LaunchDaemons /Library/LaunchAgents /System/Library/LaunchAgents 2>/dev/null >/tmp/lsat1.lsat";
	}

	if (distribution == 6)
	{
		/* slackware... I am not sure */
		tempfile = "/tmp/lsat1.lsat";
		header = "Checking services that start at boot.\n";
		shellcode = "ls -l /etc/rc.d 2>/dev/null >/tmp/lsat1.lsat";
	}

	/* we are on Mint/Ubuntu/etc */
        if ((distribution == 7) || (distribution == 8))
        {       
                /* we are on debianb/ubuntu/linuxmint */
                tempfile = "/tmp/lsat1.lsat";
                header = "Checking services that start at boot.\n";
                shellcode = "/usr/bin/dbus-send --print-reply --system --dest=com.ubuntu.Upstart /com/ubuntu/Upstart com.ubuntu.Upstart0_6.GetAllJobs 2>/dev/null |grep \"object path\"  2>/dev/null >/tmp/lsat1.lsat";
        }


    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
        /* rhut-rho...something bad happened */
        perror(" Creation of list failed.");
        return(-1);
    }
    
    if (verbose > 0)
    {
        printf(" Finished in checkservices module.\n");
    }
   
    return(0);
}
