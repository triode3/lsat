/****************************************/
/* This is the checkrc module for   	*/
/* the lsatmain.c program.              */
/* It checks /etc/rcn.d or 		*/
/* /etc/rc.d/init.d depending on sys.   */
/* It scans the startup scripts and  	*/
/* compares them to a known list of 	*/
/* scripts that one should not have.	*/
/*					*/
/* see lsatmain.c for more details	*/
/****************************************/

#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int checkrc(release, kernel, distribution, filename, verbose, html)
char release[]; /* 1st field of uname -a */
char kernel[]; /* 3rd field of uname -a */
int distribution; /* see lsatmain.c for details on this one */
const char *const filename; /* output file name */
int verbose; /* should we be verbose? */
int html;
{

    char *rcptr;		/* ptr to passwd file   */
    char line[30];		/* array for a line	*/
    const int maxlist  = 350;	/* how many filenames we can handle*/
    const int maxlength=  30;	/* how long a filename may be*/
    char temparray[maxlist][maxlength+1]; /*temparray. */
    char kernver[4][6]={"2.2", "2.4", "SunOS", "2.6"};  /* kernel versions */
    int i=0;			/* counter variable	*/
    int j=0;			/* counter variable  */
    int osflag=2;		/* what OS/kern are we at/on? */
			        /* 1= lnx2.2, 2=lnx2.4, 3=Solaris */
    int long kversion;		/* kernel version after conversion using strtol */
    char *endpointer;		/* the endpointer needed for strtol */

    /* we are going to convert the kernel string to a long int	*/
    /* and use that to compare for kernels over 3...		*/
    /* If you are still on a 2.X kernel, geez, get with the 	*/
    /* program. 						*/
    kversion = strtol(kernel, &endpointer, 10);
    /* NOT a comprehensive list yet.	*/
    /* Need to go over these to make sure...*/
    /* Linux 2.x-2 kernel */
    char lnx2list[14][14] = {"lpd", "inetd", "nfs", "nfslock", "nmb", "portmap", "rwalld", "rwhod", "rstatd", "sendmail", "smb", "ypbind", "ypasswd", "named"};
    int lnx2listnum=14; /* how many items in above list? */

    /* Linux 2.4 kernel */
    char lnx4list[18][14] = {"S05kudzu", "S13portmap", "S14nfslock", "S20nfs", "S20rstatd", "S20rwalld", "S20rwhod", "S25netfs", "S34yppasswdd",  "S50snmpd", "S60lpd", "S65identd", "S74ntpd", "S74ypserv", "S74ypxfrd", "S80sendmail", "S95anacron"};
    int lnx4listnum=17;
    char sollist[9][12] = {"S71rpc", "S76smpdx", "S77dmi", "S99dtlogin", "S74autofs", "S88sendmail", "nfs.server", "nfs.client"};
    int sollistnum=8;

    const char * header;
    const char * shellcode;
    const char * tempfile;

    FILE *outfile;		/* output file pointer 	*/
    FILE *rcfile;		/* file pointer to templist file */

    if (verbose >= 0)
    {
        printf(" Running checkrc module...\n");
    }

    /* since there is a location difference between */
    /* kernel 2.2 and 2.4, we check the kernel 	*/
    /* release we are on. Also chekc for SunOS, 	*/
    /* which has a different naming convention	*/
    if ((strcmp(kernel, kernver[0])) == 0)
    {
        /* kernel 2.2 */
        osflag=1;
    }
    if ((strcmp(kernel, kernver[1])) == 0)
    {
        /* kernel 2.4 */
        osflag=2;
    }
    if ((strcmp(release, kernver[2])) == 0)
    {
        /* SunOS */
        osflag=3;
    }
    if ((strcmp(kernel, kernver[3])) == 0)
    {
 	/* kernel 2.6, treat as 2.4 */
	osflag=2;
    }
    /* check for kernel 3 and over */
    if ((kversion >= 3))
    {
    	/* kernel version 3.X or more */
	osflag=4;
    }
	
	/* note default is assume 2.4/2.6 style... */


    if (verbose > 0)
    {
        printf(" Checking rc scripts...\n");
    }


   if (osflag == 1)
    {
        header= "The following services were found in /etc/rc.d/init.d\nCoonsider removing or disabling unnecessary services.\n";
    }
    if ((osflag == 2) || (osflag == 3) || (osflag == 4))
    {
        if (distribution == 6) /* JTO */
        {
            header = "The following services were found in /etc/rc.d\nCoonsider removing or disabling unnecessary services.\n";
        }
        if (distribution == 4) /*gentoo*/
        {
            header = "The following services were found in /etc/init.d/\nCoonsider removing or disabling unnecessary services.\n";
        }
        else
        {
            header = "The following services were found in /etc/rc(2/3).d\nCoonsider removing or disabling unnecessary services.\n";
        }
    }
    /* print out the above entry to the output file */
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.");
        return(-1);
    }


    tempfile="/tmp/lsat1.lsat";
    /* perform ls based on which sys we are on */
    /* check for gentoo 1st, then check osflag on others */
    if (distribution == 4)
    {
		shellcode="ls /etc/init.d/ 2>/dev/null >/tmp/lsat1.lsat";
    }
    else
    {
        /* 2.2 sys...	*/
        if (osflag == 1)
        { 
			/*works for SuSE, but is supposed to be /init.d/ */
	    	shellcode="ls /etc/rc.d/ 2>/dev/null >/tmp/lsat1.lsat";
	    	/* do some double escapes so C will ignore them */
            /* shellcode="find /etc/rc.d/ -exec basename \{\\} \\; 2>/dev/null >/tmp/lsat1.lsat; echo finished"; */
        }
        /* 2.4 sys... */
        /* note here we do same for Solaris. 		*/
        /* Sunos rc dirs are the same structure as 2.4, */
        /* however, we need to know if it is Solaris   	*/
        /* due to the naming convention of the files	*/
        /* in the rc directories on Solaris.		*/
        if ((osflag == 2) || (osflag == 3) || (osflag == 4) )
        {
            /* we peform as two ls's here for ease of reading in  */
            /* the file since for example an ls / /usr will put   */
            /* extra blank lines and the directory name in to the */
            /* output file...				      */
            if (distribution == 6) /* JTO */
            {
                shellcode = "ls /etc/rc.d 2>/dev/null >/tmp/lsat1.lsat";
            }
            else
            {
	        shellcode="ls /etc/rc2.d/ 2>/dev/null >/tmp/lsat1.lsat || ls /etc/rc3.d/ 2>/dev/null >/tmp/lsat1.lsat";
            }
        }
    } /* end of else */
    
    
    /* ue that shellcode to write out our file, /tmp/lsat1.lsat */
    if ((dostuff(tempfile, 0, shellcode, 0, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.");
        return(-1);
    }


    
    /* open the newly created file */
    if ((rcfile=fopen("/tmp/lsat1.lsat", "r"))==NULL)
    {
        perror(" Can't open templist...\n");
        perror(" Strange, we just created that file...\n");
        perror(" Giving up in checkrc...\n");
        return(-1);
    }

    /* open the outfile */
    outfile=fopen(filename, "a");
    if (outfile == NULL)
    {
        perror(" Can't open outfile!\n");
        perror(" Check perms or something...\n");
        perror(" Problem was in module checkrc.\n");
        return(-1);
    }


    /* grab a line from the lsat1.lsat  */
    /* store it in the temparray     */
    while (fgets(line, sizeof(line), rcfile) != NULL)
    {
        rcptr = strtok(line, ", \n");
        /* if not while. we guarentee not more than */
        /* one entry per line     */
        if (rcptr != NULL)
        {
	    /* increased %s from 30 to 60, */
	    /* as the names are large w/gentoo */
            if (sscanf(rcptr, "%60s", temparray[i]) < 1)
            {
                perror(" sscanf failed.\n");
                perror(" Failure was in module: checkrc.\n");
                return(-1);
            }
            i++;
	    if (i>maxlist)	/* too much for the static list? */
	    {
		perror (" Error in checkrc: Too many files.\n");
		return -1;
	    }
            rcptr = strtok(NULL, ", \n");
            if (rcptr == NULL)
            {
                ;
            }
        } /* end while (rcptr != NULL) */
    } /* end while (fgets...) */


    /* now we compare the entries in the temparray */
    /* to the rc scripts that are problematic.    */
    /* note that we have three lists, one for 2.2 */
    /* based kernels, one for 2.4 based kernels   */
    /* and one for SunOS/Solaris systems.	      */
    /* linux 2.2 based system */
    if ((osflag==1) || (osflag==4)) 
    {
        for (i = 0; i < maxlist; i++)
        {
            for (j = 0; j < lnx2listnum; j++)
            {
                if ((strcmp(temparray[i], lnx2list[j])) == 0)
                {
                    /* match */
                    fprintf(outfile, "%s\n", temparray[i]);
                }
            }
        }
    }

    /* linux 2.4 based system */
    if (osflag == 2)
    {
        for (i = 0; i < maxlist; i++)
        {
            for (j = 0; j < lnx4listnum; j++)
            {
                if ((strcmp(temparray[i], lnx4list[j])) == 0)
                {
                    /* match */
                    fprintf(outfile, "%s\n", temparray[i]);
                }
            }
        }
    }

    /* SunOS/Solaris based system */
    if (osflag == 3)
    {
        for (i = 0; i < maxlist; i++)
        {
            for (j = 0; j < sollistnum; j++)
            {
                if ((strcmp(temparray[i], sollist[j])) == 0)
                {
                    /* match */
                    fprintf(outfile, "%s\n", temparray[i]);
                }
            }
        }
    }

    /* clean up */
    fclose(outfile);
    /* close input file */
    fclose(rcfile);
    /* rm the input file */
    if (system("rm -f /tmp/lsat1.lsat") < 0)
    {
        perror(" Can't rm tempfile??\n");
        perror(" Strange, we made the file...\n");
        perror(" Giving up in checkrc...\n");
        return(-1);
    }


    if (verbose > 0)
    {
        printf(" Finished in checkrc module.\n");
    }
    return(0);
}
