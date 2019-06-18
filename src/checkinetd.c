/************************************************/
/* This checks inetd.conf for baddies...	*/
/* This function checks inetd.conf or xinetd.d  */
/* for unwanted services. It outputs the list   */
/* of all servies running (all are unwanted, 	*/
/* right?) to the outfile.			*/
/* If inetd.conf exists or if xinetd.d is 	*/
/* present it also returns a 1 to main to tell  */
/* main to run a check of /etc/hosts.deny.	*/
/*						*/
/* See lsatmain.c for more details...		*/
/************************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int checkinetd(filename, distribution, verbose, html)
const char *const filename;
int distribution;
int verbose;
int html;
{

    char *ptr;
    char line[128];        /* temp line for reading   */
    char tempstring[128];    /* tempstring for inet     */
    int inetd  = 0;		/* do we use inetd?        */
    int xinetd = 0;		/* do we use xinetd? 	   */
    int linecount, field;   /* some vars...		   */
    int emptyflag=1;	/* is inet an empty file?  */
    FILE *infile;  		/* tempfile file pointer   */
    FILE *fileptr;

    const char * tmp_file = "/tmp/lsat2.lsat"; /* temp file */

    const char * tempfile =NULL;
    const char * shellcode=NULL;
    const char * header   =NULL;


    if (verbose >= 0)
    {
        printf(" Running checkinetd module...\n");
    }

    /* really check if /etc/inetd.conf or /etc/xinetd.conf exist */
    /* instead of guessing */

    if (distribution != 3)
    /* we are not on Solaris... */
    {
        /* check for /etc/inetd.conf */
        if (system("test -e /etc/inetd.conf")==0)
        {
	    inetd=1;
        }
    
        /* check for /etc/xinetd.d/ */
        if (system("test -e /etc/xinetd.d")==0)
        {
	    xinetd=1;
        }
    }
   
    if (distribution == 3)
    /* we are on Solaris, test -e does not work under sh */
    {
        /* check for /etc/inetd.conf */
        if (system("test -f /etc/inetd.conf")==0)
	{
	     inetd=1;
        }

        /* check for /etc/xinetd.d/ */
        if (system("test -f /etc/xinetd.d")==0)
        {
	    xinetd=1;
 	}
    }


    /* now we start to check */
    if (inetd == 1)
    {
        if (verbose > 0)
        {
            printf(" Checking entries in /etc/inetd.conf...\n");
        }

        /* note: grep returns
        *  0 on match
        *  1 on no match
        *  2 on system/file/read/other failure
        *  HOWEVER, -v inverts the sense of 0,1.
        */

        header = "inetd.conf entries:\n";
        tempfile = "/tmp/lsat1.lsat";

        /* use different shellcodes as linux grep is */
		/* mo-betta than standard Solaris grep  :)   */
        if (distribution != 3)
		{
	    	/* find all lines that do not _start_ with a "#" */
	    	shellcode = "grep -Ev ^\\[:space:\\]*\\# /etc/inetd.conf >/tmp/lsat1.lsat";
		}
        if (distribution == 3)
        {
	    /* find all lines that do not _start_ with a "#" */
	    /* Note, either I suck with grep, or Solaris grep*/
            /* is drain bramaged... have to just check for # */

            shellcode = "grep -v \\# /etc/inetd.conf >/tmp/lsat1.lsat";
        }
        if ((dostuff(tempfile, 0, shellcode, header, html)) < 0)
        {
	    /* something went wrong */
	    perror(" Creation of list failed.");
	    return(-1);
        }

        /* now instead of reading the whole file and 	*/
        /* say looking for baddie servies, I consider	*/
        /* that all in there are evil. Merely tell the  */
        /* user what we have found that is open.        */
        /* perhaps also tell them to close them. :)     */
        /* note that if the file is empty, thats great. */
        /* it _should_ mean that they have commented    */
        /* out everything in /etc/inetd.conf		*/

        infile = fopen("/tmp/lsat1.lsat", "r");
        if (infile == NULL)
        {
            perror(" Error opening tmp file.\n");
            return(-1);
        }

        /* create another tmp file for entries... */
        if ((fileptr = fopen(tmp_file, "a")) == NULL)
		{
            perror(" Could not create temporary file...\n");
            perror(" Possible link attack while creating/opening file!\n");
            exit(-1);
        }
	/* init linecount */
	linecount = 0;
	/* init emptyflag */
	emptyflag = 0;

        while (fgets(line, sizeof(line), infile) != NULL)
        {
            field=0;
            ptr = strtok(line, ", \"\n");
            if (ptr == NULL)
            {
                if (linecount == 0)
                {
                    /* 1st line was empty, must have been */
                    /* an empty file. This _should_ be a  */
                    /* "good thing"....		      */
                    /* set flag, then break.	      */
                    emptyflag = 1;
                }
            }
            linecount++;
            while (ptr != NULL)
            {
                if (sscanf(ptr, "%127s", tempstring) < 1)
                {
                    perror(" sscanf bit the dust");
                    perror(" Dust biting was in checkinetd.\n");
                    return(-1);
                }
                /* note: we just need the 1st field... */
                if (field == 0)
                {
                    fprintf(fileptr, "%s\n", tempstring);
                }
                ptr = strtok(NULL, ", \"\n");
                if (ptr == NULL)
                {
                    ;
                }
                field++;
            }
        } /* end while(fgets...) */

    	/* close files in use */
    	fclose(infile);
    	/* close the tmp file, do not rm it */
		fclose(fileptr);
    	/* rm tempfile since dostuff will be called */
    	/* with the new tmpfile we created	*/
    	remove(tempfile);


    	/* call dostuff to append the new tmpfile */
    	/* to the outfile.			*/
    	if (emptyflag == 1)
    	{
		/* no baddies in inetd.conf */
        	header = "No non-essential items found in inetd.conf. Good.\n";
    	}
    	if (emptyflag == 0)
		{
	        /* found some lines not commented out... */
            	header = "Please comment out all of these from inetd.conf.\n";
		}
    	if ((dostuff(tmp_file, filename, 0, header, html)) < 0)
		{
	    /* something went wrong */
	    perror(" Creation of list failed.");
	    return(-1);
		}
    } /* end of if(inetexists==1) */


    /* end of RedHat 5.x, 6.x check portion */
    /* note that in the above dostuff it should	*/
    /* have removed the tempfile, so we will    */
    /* reuse it below for xinetd.d entries...   */


    /* if we are on RedHat 7.x we use xinetd.d/ files */
    if (xinetd == 1)
    {

        if (verbose > 0)
        {
            printf(" Checking entries in /etc/xinetd.d/...\n");
        }

        /* init the linecount here   */
        linecount = 0;


        /* assume RedHat 7.x box */
        /* read through the files in /etc/xinetd.c and	*/
        /* check for ones that are not disabled.	*/
        tempfile = "/tmp/lsat1.lsat";
        shellcode = "grep disable /etc/xinetd.d/* 2>/dev/null |grep no 2>/dev/null |awk -F\" \" 'length($1 $4) > 0 {print($1 $4)}' >/tmp/lsat1.lsat";
        header = "Entries below shall be services in xinetd.d that\nare not disabled.\nPlease verify that you do not want these disabled.\nIf nothing is listed below, all services in xinetd.d are disabled.\n";
        if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
        {
	    /* something went wrong */
	    perror(" Creation of list failed.");
	    return(-1);
	}
    } /* end of RedHat 7.x check portion */

    /* the xinetd.conf file may not exist on some systems (aka gentoo) */
    if ((system("test -r /etc/xinetd.conf"))==0)
    {
        if ((system("grep only_from /etc/xinetd.conf 1>/dev/null 2>/dev/null >/dev/null") == 0))
        {
	    /* found only_from in xinetd.conf, tell user to check it */
            /* later on I will check against the ifconfig output or  */
	    /* something *smile* 					 */
            header = "Found only_from line in xinetd.conf.\nMake sure this matches what you want to allow\n";
        }
        else 
        {
 	    /* did not find only_from in xinetd.conf... user should */
 	    /* add this to xinetd.conf...				*/
	    header = "Did not find only_from= in /etc/xinetd.conf.\nPlease add this to allow subnets that you want to give access to.\n";
        }
        if ((dostuff(0, filename, 0, header, html)) < 0)
        {
            /* something went wrong */
            perror(" Creation of list failed.");
            return(-1);
        }
    }	

    if (verbose > 0) 
    {
        printf(" Finished module checkinetd.\n");
    }

    /* instead of just returning 0 here */
    /* we return (inetd or xinetd)... 	    */
    /* 0 for no existance, 1 if either exists  */
    return(0);
}
