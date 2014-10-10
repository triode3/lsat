/****************************************/
/* This is the checkipv4 module ...it   */
/* it checks for various proc entries   */
/* that aid in a more secure system...  */
/*					*/
/* See lsatmain.c for more details.	*/
/****************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int checkipv4(filename, verbose, html)
const char *const filename;
int verbose;
int html;
{
    const char * header   =NULL;

    if (verbose >= 0)
    {
    	printf(" Running checkipv4 module...\n");
    }

    /* check to see if we ignore pings... */
    if ((system("test -r /proc/sys/net/ipv4/icmp_echo_ignore_all 1>/dev/null")) == 0)
    {
        /* file is there, check it */
        if ((system("grep 0 /proc/sys/net/ipv4/icmp_echo_ignore_all 1>/dev/null")) == 0)
        {
            /* it has a zero, we want a one in it. */
            header = "/proc/sys/net/ipv4/icmp_echo_ignore_all exists, but is off.\nConsider enabling it by placing a one in it.\nYou can do this with: echo \"1\" > /proc/sys/net/ipv4/icmp_echo_ignore_all\n";
        }
        else 
        { 
            /* its there and has a one. good. */
            header = "You ignore all ICMP Echo requests, good.\n";
        }
    }
    else
    {
        /* file is not there at all... */
        header = "Consider ignoring icmp_echo...\n";
    }
    /* ok, call dostuff to print out the header */
    /* to the output file... 			*/
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* something went wrong */
	perror(" Creation of list failed.");
	return (-1);
    }

    /* check to see if we ignore broadcasts... */
    if ((system("test -r /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts 1>/dev/null")) == 0)
    {
         /* file is there, check it */
         if ((system("grep 0 /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts 1>/dev/null")) == 0)
         {
              /* it has a zero, we want a one in it. */
              header = "/proc/sys/net/ipv4/icmp_echo_ignore_broadcasts exists but it is off\nConsider enabling it by placing a one in it.\nYou can do this with: echo \"1\" > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts\n";
         }
         else
         {
              /* its there and has a one. good. */
              header = "You ignore all ICMP Echo broadcasts, good.\n";
         }
    }
    else
    {
         /* file is not there at all... */
         header = "Consider turning on icmp_echo_ignore_broadcasts.\n";
    }

    /* ok, call dostuff to print out the header */
    /* to the output file...                    */
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
         /* something went wrong */
         perror(" Creation of list failed.");
         return (-1);
    }

    /* check to see if we disable source routed packets */
    if ((system("test -r /proc/sys/net/ipv4/conf/all/accept_source_route 1>/dev/null")) == 0)
    {
        /* it exists, check it */
        if ((system("grep 1 /proc/sys/net/ipv4/conf/all/accept_source_route 1>/dev/null")) == 0)
        { 
            /* its there and has a one, we want a zero */
            header = "/proc/sys/net/ipv4/conf/all/accept_source_route exists, but it is off.\nConsider disabling source routing by placing a zero in it.\nYou can do this with: echo \"0\" > /proc/sys/net/ipv4/conf/all/accept_source_route\n";
        }
        else
        {
            /* its there and its on */
            header = "You are denying source routed packets. Good.\n";
        }
    }
    else
    {
        /* file is not there at all... */
        header = "Consider denying source routed packets.\n";
    }
    /* ok, call dostuff to print out the header */
    /* to the output file...                    */
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.");
        return (-1);
    }

    /* check to see if we disable redirect acceptance */
    if ((system("test -r /proc/sys/net/ipv4/conf/all/accept_redirects 1>/dev/null")) == 0)
    {
        /* its there check it */
        if ((system("grep 1 /proc/sys/net/ipv4/conf/all/accept_redirects 1>/dev/null")) == 0)
        {
            /* its there and has a one, we want a zero. */
            header = "/proc/sys/net/ipv4/conf/all/accept_redirects exists, and you are accepting redirects.\nConsider disabling redirects by placing a zero in it.\nYou can do this with: echo \"0\" > /proc/sys/net/ipv4/conf/all/accept_redirects\n";
        }
        else
        {
            /* its there and it has a one */
            header = "You are not accepting ipv4 redirects. Good\n";
        }
    }
    else
    {
        /* that file does not even exist */
        header = "Consider not accepting ipv4 redirects.\n";
    }
    /* ok, call dostuff to print out the header */
    /* to the output file...                    */
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.");
        return (-1);
    }

    /* check to see if we are protecting against bad err msgs */
    if ((system("test -r /proc/sys/net/ipv4/icmp_ignore_bogus_error_responses")) == 0)
    { 
        /* its there, check it */
        if ((system("grep 0 /proc/sys/net/ipv4/icmp_ignore_bogus_error_responses 1>/dev/null")) == 0)
        {
            /* its has a zero, we want a one */
            header = "IPV4 protection against bad err msgs is there, but it is off.\nConsider enabling it by placing a 1 in it.\nYou can do this with: echo \"1\" > /proc/sys/net/ipv4/icmp_ignore_bogus_error_responses\n";
        }
        else
        {
            /* its there and on */
            header = "You are ignoring bad err msgs in ipv4. Good.\n";
        }
    }
    else
    {
        /* file was not there at all */
        header = "You are not ignoring bad error msgs in ipv4.\nConsider turning this on.\n";
    }
    /* ok, call dostuff to print out the header */
    /* to the output file...                    */
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.");
        return (-1);
    }

/* I am considering checking for reverse path filtering, but hesitate */
/* to add it just yet... */

    /* check for logging of spoofed, source routed and redirected pkts */
    if ((system("test -r /proc/sys/net/ipv4/conf/all/log_martians 1>/dev/null")) == 0)
    {
        /* it exists, check it */
        if ((system("grep 0 /proc/sys/net/ipv4/conf/all/log_martians 1>/dev/null")) == 0)
        {
            /* its there but off */
            header = "Logging of spoofed, etc packets is off.\nConsider turning on.\nYou can do this with: echo \"1\" > /proc/sys/net/ipv4/conf/all/log_martians\n";
        }
        else
        {
            /* its on already. */
            header = "You are logging spoofed, etc packets. Good.\n";
        }
    }
    else
    {
        /* that file does not exist... */
        header = "Consider turning on the logging of spoofed, source routed\nand redirected ipv4 packets.\n";
    }
    /* ok, call dostuff to print out the header */
    /* to the output file...                    */
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.");
        return (-1);
    }

    
    if (verbose > 0)
    {
        printf(" Finished in checkipv4 module.\n");
    }

    return(0);
}
