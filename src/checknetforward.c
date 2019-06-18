/************************************************/
/* This module checks to see that we are not    */
/* doing IP forwarding on the local machine.    */
/* It basically just checks the ip_forward and  */
/* ip_dynaddr files for 0 and also the network  */
/* file in /etc/sysconfig should contain the    */
/* line: FORWARD_IPV4=FALSE....			*/
/*						*/
/* See lsatmain.c for more details...		*/
/************************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int checknetforward(filename, distribution, verbose, html)
const char *const filename;
int distribution;
int verbose;
int html;
{

    const char * header   =NULL;

    if (verbose >= 0)
    {
        printf(" Running checknetforward module...\n");
    }

    if (verbose > 0)
    {
        printf(" Checking for existance of forwards...\n");
    }

    /* on redhat/mandrake/slack check /proc/sys/net/ipv4/ip_forward  */
    /* & ip_dynaddr also check /etc/sysconfig/network */

    if ((distribution != 3) && (distribution != 5))
    {

        if (verbose > 0)
	{
	    printf(" Checking ip_forward...\n");
	}
        if ((system("grep 0 /proc/sys/net/ipv4/ip_forward 1>/dev/null 2>/dev/null")) != 0)
        {
		/* no 0 in ip_forward */
                header = "\nLooks like ip_forward is enabled on this system\nensure /proc/sys/net/ipv4/ip_forward\ncontains a 0 only as its entry.\n";
                if ((dostuff(0, filename, 0, header, html)) < 0)
	 	{
		    /* something went wrong */
		    perror(" Writing to output failed.");
		    return(-1);
 		}
	}
        if (verbose > 0)
	{
	    printf(" Checking network file...\n");
	}

        /* first see if /etc/sysconfig/network exists... */
	if ((system("test -r /etc/sysconfig/network 1>/dev/null 2>/dev/null")) == 0)
	{
            if ((system("grep FORWARD_IPV4=FALSE /etc/sysconfig/network 1>/dev/null 2>/dev/null")) !=0)
            {
                /* no FALSE for IPforwarding */
                header = "Hrm, do not see FORWARD_IPV4=FALSE in network.\nMake sure that /etc/sysconfig/network\ncontains the line FORWARD_IPV4=FALSE\n";
                if ((dostuff(0, filename, 0, header, html)) < 0)
                {
                    /* something went wrong */
                    perror(" Writing to output failed.");
                    return(-1);
                }
	    }
        }
    }

    /* Assume Solaris/SunOS */
    if ((distribution == 3))
    {
        if (verbose > 0)
	{
	    printf(" Checking ip_forwarding.\n");
	}
        if ((system("grep \'ip_forwarding 0\' /etc/rc2.d/S69inet 1>/dev/null 2>/dev/null")) != 0)
        {
                header = "Make sure that ip_forwarding is disabled\nin /etc/rc2.d/S69inet\n";
		if ((dostuff(0, filename, 0, header, html)) < 0)
                {
                    /* something went wrong */
                    perror(" Writing to output failed.");
                    return(-1);
                }
	}
	if (verbose > 0)
	{
	    printf(" Checking ip_forwarding_src_routed.\n");
	}
	if ((system("grep \'ip_forwarding_src_routed 0\' /etc/rc2.d/S69inet 1>/dev/null 2>/dev/null")) != 0)
	{
		header = "Make sure that ip_forwarding_src_routed is disabled\nin /etc/rc2.d/S69inet\n";
		if ((dostuff(0, filename, 0, header, html)) < 0)
                {
                    /* something went wrong */
                    perror(" Writing to output failed.");
                    return(-1);
                }
	}
	if (verbose > 0)
	{
	    printf(" Checking ip_forwarding_directed_broadcast\n");
	}
	if ((system("grep \'ip_forwarding_directed_broadcast 0\' /etc/rc2.d/S69inet 1>/dev/null 2>/dev/null")) != 0)
	{
		header = "Make sure that ip_forwarding_directed_broadcast is disabled\nin /etc/rc2.d/S69inet\n";
		if ((dostuff(0, filename, 0, header, html)) < 0)
                {
                    /* something went wrong */
                    perror(" Writing to output failed.");
                    return(-1);
                }
	}
	if (verbose > 0)
	{
	    printf(" Checking norouter file.\n");
	}
	if ((system("cat /etc/norouter 1>/dev/null 2>/dev/null")) != 0)
	{
		header = "Please touch /etc/norouter\n";
		if ((dostuff(0, filename, 0, header, html)) < 0)
                {
                    /* something went wrong */
                    perror(" Writing to output failed.");
                    return(-1);
                }
	}
	if (verbose > 0)
	{
	    printf(" Checking defaultrouter file.\n");
	}
	if ((system("cat /etc/defaultrouter 1>/dev/null 2>/dev/null")) != 0)
	{
		header = "Please touch (at least) /etc/defaultrouter\n";
		if ((dostuff(0, filename, 0, header, html)) < 0)
                {
                    /* something went wrong */
                    perror(" Writing to output failed.");
                    return(-1);
                }
	}
    }

    if (distribution == 5)
    {
	/* Mac OSX */
	if ((system("grep IPFORWARDING /etc/hostconfig  | grep YES 1>/dev/null 2>/dev/null")) != 0)
        {
	    /* it should be no in that case... good*/
	    header = "IPFORWARDING not enabled...good.\n";
        }
        else
        {
	    header = "IPFORWARDING is enabled!\nEdit /etc/hostconfig to disable.\n";
	}
        if ((dostuff(0, filename, 0, header, html)) < 0)
        {
            /* something went wrong */
            perror(" Writing to output failed.");
            return(-1);
        }
    }



    if (verbose > 0)
    {
        printf(" Finished module checknetforward.\n");
    }
    return(0);
}
