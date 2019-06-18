/********************************************/
/* This module will (hopefully) check to see*/
/* if loadable modules are enabled in the   */
/* kernel...for a really secure system, this*/
/* should be disabled and they should be    */
/* static in the kernel. 		    */
/*                                          */ 
/* This will (obviously) not work under     */
/* Solaris or AIX...			    */
/* Note I will check in lsatmain.c to see   */
/* if we are on a linux distro before we    */
/* run this. I think this will save some    */
/* cpu cycles instead of coming here to do  */
/* this check...		   	    */
/* 					    */
/* See lsatmain.c for more details.         */
/********************************************/
#include "lsatheader.h" 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int checkmodules(filename, verbose, html)
const char *const filename;
int verbose;
int html;
{
    const char * header   =NULL;
    const char * tempfile =NULL;
    const char * shellcode=NULL;
  
    if (verbose >= 0)
    {
        printf(" Running checkmodules module...\n");
    }


    if ((system("cat /proc/modules 1>/dev/null 2>/dev/null >/dev/null")) == 0 )
    {

        if ((system("grep module /proc/ksyms 1>/dev/null 2>/dev/null >/dev/null")) == 0)
        {

		if (system("/sbin/lsmod 2>&1|grep -e QM_MODULES") == 0)
		{
			tempfile = "/tmp/lsat1.lsat";
			header = "Modules appear to be enabled in the kernel.\nList of modules loaded:\n"; 
			shellcode = "/usr/bin/strings -an1 /proc/modules 2>&1>/tmp/lsat1.lsat";
				if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
				{
				/* rhut-rho...something bad happened */
				perror(" Creation of list failed.");
				return(-1);
				}
		} else { 
			tempfile = "/tmp/lsat1.lsat";
			header = "Modules appear to be enabled in the kernel.\nList of modules loaded:\n";
			shellcode = "/sbin/lsmod 2>&1>/tmp/lsat1.lsat";
				if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
				{
				/* rhut-rho...something bad happened */
				perror(" Creation of list failed.");
				return(-1);
				}

		}
	}
      }

    if ((system("test -r /sbin/modprobe")) == 0)
    {
    	shellcode = "/sbin/modprobe -c -l 2>/dev/null >/tmp/lsat1.lsat";
    	header = "These are the kernel modules that are loaded on the system\nas given by the output of modprobe -c -l\nCheck to see if they are really needed.\n";
	tempfile = "/tmp/lsat1.lsat";
	if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
	{
	    /* something went wrong */
	    perror(" Creation of list failed.");
	    return(-1);
	}
    }

    /* I should just test for solaris here */
    if ((system("test -r /usr/sbin/modinfo")) == 0)
    {
        shellcode = "/usr/sbin/modinfo 2>/dev/null >/tmp/lsat1.lsat";
	header = "These are kernel modules that are loaded on the system\nas given by the output of modinfo.\nCheck to see if they are really needed.\n";
	tempfile = "/tmp/lsat1.lsat";
	if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
	{
	    /* something went wrong */
	    perror(" Creation of list failed.");
	    return(-1);
	}
    }



    if (verbose > 0)
    {
        printf(" Finished in checkmodules module.\n");
    }

    return(0);
}

