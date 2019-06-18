/********************************************/
/* This will check open files on the        */ 
/* system using lsof. This is a little      */
/* out of control, but doing this and       */
/* comparing to a previous output could     */
/* prove to be useful.                      */
/*                                          */
/* this module will later be expanded to    */
/* write a temp file in the CWD, and then   */
/* upon subsequent runs, it will diff them  */
/* and print that output. That would prove  */
/* much more useful.                        */
/*                                          */ 
/* See lsatmain.c for more details.         */
/********************************************/
#include "lsatheader.h" 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int checkopenfiles(filename, distribution, verbose, html)
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
        printf(" Running checkopenfiles module...\n");
    }

    /* it was noted that some distros do not install lsof by    */
    /* default, mainly gentoo... we therefore check here.       */
    if (distribution != 3)
    {
        if ((system("lsof -i 1>/dev/null 2>/dev/null >/dev/null")) != 0 ) 
        {
            /* perhaps lsof is not installed */
	    header = "lsof is not installed on this system,\nor it is not in the path,\nor I just can not find it.\ncheckopenfiles was not run.\n";
	    lsof_installed = 1;
        }
    }
    
    /* this is rediculous. If Solaris, we don't know if lsof is */
    /* installed, however, which does not seem to return a value*/
    /* and the shell does not seem to want to either on calling */
    /* a command that is not there... we therefore do this.     */
    /* QOTD: we ran find already, so it should be fast...       */
    if (distribution == 3)
    {
        if ((system("find / -name lsof 1>/dev/null 2>/dev/null >/dev/null")) != 0)
        /* then we think lsof is not installed or not in the path */
        {
	    header = "lsof is not installed on this system,\nor it is not in the path,\nor I just can not find it.\ncheckopenfiles was not run.\n";
            lsof_installed = 1;
        }
    } 

    if (lsof_installed == 0)
    {
    	header = "This is the lsof output, diff this against a previous run.\n";
    	tempfile = "/tmp/lsat1.lsat";
    	shellcode = "lsof -i 2>/dev/null >>/tmp/lsat1.lsat";
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
        printf(" Finished in checkopenfiles module.\n");
    }

    return(0);
}

