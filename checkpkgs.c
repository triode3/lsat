/****************************************/
/* This is the checkpkgs module for the */
/* lsatmain.c program. It checks the    */
/* currently installed pkgs against a   */
/* (somewhat small) list of unwanted 	*/
/* rpms. It then prints out this list   */
/* to the outfile.			*/
/*					*/
/* see lsatmain.c for more details	*/
/****************************************/

#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int checkpkgs(distribution, filename, verbose, html)
int distribution;
const char *const filename;
int verbose;
int html;
{

    /* note if distribution = 1, redhat */
    /* if distribution = 2, debian */
    /* if distribution = 4, gentoo */
    /* if distribution = 3, solaris, not implemented yet */
    char *templistptr;	/* ptr to templist file */
    char line[80];		/* array for a line	*/
    char temp1[80];		/* temp char 		*/
    int i=0;		/* counter variable	*/
    int submatch=0;		/* match flag		*/

    /* list of rpms to check? 		*/
    /* NOT a comprehensive list yet.	*/
    /* just making sure this works...	*/
    char rpmlist[17][15] = {"bind", "identd", "lpd", "linuxconf", "netfs", "nfs", "portmap", "routed", "rstatd", "rusersd", "rwalld", "rwhod", "sendmail", "webmin", "ypbind", "yppasswdd", "ypserv"};

    FILE *templist;		/* file pointer to templist file */
    FILE *fileptr;
    const char * tmp_file = "/tmp/lsat2.lsat"; /* tmp file for output */

    const char * tempfile =NULL;
    const char * shellcode=NULL;
    const char * header   =NULL;

    if (verbose >= 0)
    {
        printf(" Running checkpkgs module...\n");
    }

    /* ok, if we are on Mac OSX, we (as of yet) have no
    * cool built in for this, so we basically bail out */
    if (distribution == 5)
    {
	/* we are on Darwin */
	;
    }
    else
    {
    /* tempfile for either case */
    /* note carefully, the above tmp_file is for the second tempfile */
    tempfile = "/tmp/lsat1.lsat"; 

    /* if distro = redhat, use rpm */
    if (distribution == 1)
    {
        if (verbose > 0)
		{
	    	printf(" Generating list of rpms on system.\n");
		}
        shellcode = "rpm -qa >> /tmp/lsat1.lsat";
        if ((dostuff(tempfile, 0, shellcode, 0, html)) < 0)
        {
            /* rhut-rho...something bad happened */
            perror(" Creation of list failed.");
            return(-1);
        }
    }

    /* if distro = debian, use dpkg */
    if ((distribution == 2) || (distribution == 7) || (distribution == 8))
    {
		if (verbose > 0)
		{
	    	printf(" Generating list of pkgs on system.\n");
		}
        shellcode = "dpkg -l |awk -F\" \" 'length($2) > 0 {print $2}' 1>/dev/null 2>/dev/null >> /tmp/lsat1.lsat";
        if ((dostuff(tempfile, 0, shellcode, 0, html)) < 0)
        {
            /* rhut-rho...something bad happened */
            perror(" Creation of list failed.");
            return(-1);
        }
    }

    /* if distro = solaris, use pkginfo */
    if (distribution == 3)
    {
        if (verbose > 0)
        {
            printf(" Generating list of pkgs on system.\n");
        }
        shellcode = "pkginfo -x |awk -F" " 'length($1) > 0 {print $1}' 1>/dev/null 2>/dev/null >> /tmp/lsat1.lsat";
        if ((dostuff(tempfile, 0, shellcode, 0, html)) < 0)
		{
            /* rhut-rho...something bad happened */
            perror(" Creation of list failed.");
            return(-1);
		}
    }


    /* if distro = gentoo, use pkglist */
    if (distribution == 4)
    {
        if (verbose > 0)
        {
            printf(" Generating list of pkgs on system.\n");
        }
        shellcode = "/usr/lib/portage/bin/pkglist 2>/dev/null >>/tmp/lsat1.lsat";
        if ((dostuff(tempfile, 0, shellcode, 0, html)) < 0)
        {
            /* rhut-rho...something bad happened */
            perror(" Creation of list failed.");
            return(-1);
        }
    }

    /* if distro = slackware, use ls */
    if (distribution == 6) /* JTO */
    {
        if (verbose > 0)
        {
            printf(" Generating list of pkgs on system.\n"); /* JTO - changed 'rpms' to 'pkgs' */
        }
        shellcode = "ls /var/log/packages >>/tmp/lsat1.lsat";
        if ((dostuff(tempfile, 0, shellcode, 0, html)) < 0)
        {
            /* rhut-rho...something bad happened */
            perror(" Creation of list failed.");
            return(-1);
        }
    }

    /* open the file we just made for read */
    templist=fopen("/tmp/lsat1.lsat", "r");
    if (templist == NULL)
    {
        perror(" Can't open /tmp/templist.txt!!\n");
        perror(" Check perms or something...\n");
        perror(" Error was in module: checkpkgs.\n");
        return(-1);
    }

    /* create another tmp file for putting items in */
    if ((fileptr = fopen(tmp_file, "a")) == NULL)
    {
        perror(" Could not create temporary file...\n");
        perror(" Possible link attack while creating/opening file!\n");
        exit(-1);
    } 

    if (verbose > 0)
    {
        printf(" Checking rpm list...\n");
    }


    /* ok, for each entry in rpmlist we want to  */
    /* check if that exitst in templist...       */

    /* to speed things up, we only read through  */
    /* templist once, for each line in it        */
    /* we compare to all in the rpmlist...       */

    while (fgets(line, sizeof(line), templist) != NULL)
    {
        templistptr = strtok(line, ", \n");
        if (templistptr != NULL)
        {
            if (sscanf(templistptr, "%79s", temp1) < 1)
            {
                perror(" sscanf failed.\n");
                perror(" Failure in module: checkpkgs.\n");
                return(-1);
            }
            for (i=0; i<16; i++)
            {
                if (strstr(temp1, rpmlist[i]) != NULL)
                {
                    /* substring matched */
                    /* set flag	     */
                    submatch = 1;
                    /* print match out   */
		    fprintf(fileptr, "%s\n", temp1);
                }
            }
        }
    }

    /* close the templist */
    fclose(templist);
    /* rm it since dostuff will work with the second one */
    remove(tempfile);
    /* close the second temp file */
    fclose(fileptr);

    if (submatch == 1)
    {
        header = "Please consider removing these packages.\n";
    }
    if (submatch == 0)
    {
        header = "No installed rpms matched my list of non-essentials. Good.\n";
    }

    if ((dostuff(tmp_file, filename, 0, header, html)) < 0)
    {
	perror(" Write to outfile failed.");
	return(-1);
    }


    /* this curly ends the else from the start where we see if this is Mac OSX */
    }

    if (verbose > 0)
    {
        printf(" Finished in checkpkgs module.\n");
    }
    return(0);
}
