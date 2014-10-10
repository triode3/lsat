/********************************************/
/* This module will find all files on the   */
/* system and print an md5sum for each one  */
/* out to a file...not the lsat.out file.   */
/* In this way the output can be diffed     */
/* against an older one. If this module was */
/* already run, the old outfile will be     */
/* renamed to .old. I may put in a switch   */
/* to automagically diff the output....     */
/* 					    */
/* See lsatmain.c for more details.         */
/********************************************/

/********************************************/
/* 06/18/2014:				    */
/* It seems a number of distributions are   */
/* starting to include shaNNNsum by default.*/
/* In light of this, we will first test for */
/* the existence of shaNNNsum and use it if */
/* it is installed, and only use md5sum as  */
/* a fallback mechanism.                    */
/* NOTE: I will leave the name of the module*/
/* at checkmd5sum but I will call the output*/
/* by the name of the sum that was run...   */
/* e.g. md5, sha512, sha256, etc.           */
/********************************************/

/********************************************/
/* Special note for md5sum (GNU/Linux)      */
/* 10/20/2003:                              */
/*                                          */
/* md5sum is "sortof" broken... who is at   */
/* fault is really a matter of perspective, */
/* but nevertheless, if one tries to md5sum */
/* an "unreadable" file (say /proc/kmsg),   */
/* md5sum will hang. This is due to the fact*/
/* that you can not really read the file,   */
/* or better yet, the first few characters. */
/* e.g. if you cat /proc/kmsg (redhat 9.0)  */
/* and strace it, you will receive a read   */
/* error on the first two bytes. This causes*/
/* md5sum to hang. Even though they are     */
/* checking to make sure the line pointer is*/
/* good in md5sum.c, they do not check if   */
/* the first few bytes are actually readable*/
/* For this reason, until I can come up with*/
/* a good solution, (and then convincing the*/
/* md5sum maintainer is another thing) or   */
/* think of a good test in here, I will be  */
/* ignoring the /proc and /dev directories  */
/* during the find for md5summing files...  */
/* yuk.                                     */
/*                                          */
/* I will try to work on this and fix it as */
/* soon as I can.                           */
/********************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int checkmd5(filename, distribution, verbose, html, diff)
const char *const filename;
int verbose;
int distribution;
int html;
int diff;
{
    int outexists = 0;
    int sumtype = 0; /* 0 = md5sum, 1 = sha512sum */
    const char * header   =NULL;
    const char * tempfile =NULL;
    const char * outfile2 =NULL;
    const char * shellcode=NULL;
    const char *outputfile =NULL; 
    char *newname = NULL;
    FILE *tempptr;
  
    if (verbose >= 0)
    {
        printf(" Running checkmd5 module...\n");
    }

    /* ok, make a temp file for holding the output */
    tempfile="/tmp/lsat1.lsat";
 
    /* lets get a list of files and md5(sum) them... */
    /* denote again... under Mac OSX its md5 and not md5sum... */
    if (distribution == 5)
    {
        /* OK, since md5 is hacked and OSX includes shasum,    */
	/* lets try to use that first. IF we do, change headers*/
	 if ((system("test -r /usr/bin/shasum")) == 0)
	 {
	 	sumtype = 1;
	 	/* it is there, let us call the output lsatsum...     */
	 	header = "Check lsatsum.out for output of checksum.\nIf this is a subsequent run, the old one is called lsatsum.out.old.\n";
	 	if ((dostuff(0, filename, 0, header, html)) < 0)
	 	{
	 		/*something went wrong */
			perror(" Creation of list failed.\n");
			return(-1);
	 	}
	 	shellcode = "find / -type f -print0 2>/dev/null |xargs -0 shasum -a 256  2>/dev/null >> /tmp/lsat1.lsat";
	 }
	 else
	 { 
	 	/* use md5sum */
                header = "check lsatmd5.out for output of checkmd5.\nIf this is a subsequent run, old one is called lsatmd5.out.old\n";
                if ((dostuff(0, filename, 0, header, html)) < 0)
                {
                        /* something went wrong */
                        perror(" Creation of list failed.\n");
                        return(-1);
                }   

	 	shellcode = "find / -type f -print0 2>/dev/null |xargs -0 md5 2>/dev/null >> /tmp/lsat1.lsat";
	 }
    }
    else /* we are no longer talking about OSX */
    {
	/* Again, lets test for shasums... */
	if ((system("test -r /usr/bin/sha512sum")) == 0)
	{ 
		sumtype = 1;
		/* ok lets use sha512 */
		header = "check lsatsum.out for output of checksum.\nIf this is  a subsequent run, the old one is called lsatsum.out.old\n";
                if ((dostuff(0, filename, 0, header, html)) < 0)
                {
                        /* something went wrong */
                        perror(" Creation of list failed.\n");
                        return(-1);
                } 
		shellcode = "find / -path /dev/ -prune -o -path /proc -prune -o -path /home -prune -o -path /var -prune -o -path /tmp -prune -o -path /sys -prune -o -type f -exec sha512sum \\{\\} \\; 2>/dev/null >> /tmp/lsat1.lsat";
	}
	else
	{
		/* use md5sum */
		header = "check lsatmd5.out for output of checkmd5.\nIf this is a subsequent run, old one is called lsatmd5.out.old\n";
		if ((dostuff(0, filename, 0, header, html)) < 0)
		{
		        /* something went wrong */
		        perror(" Creation of list failed.\n");
		        return(-1);
	        }

        /* ugh, that is silly. */
	/* yes, let's make it ((silly--)) */
	/* this one really avoids proc dev and also var and tmp  and /sys */
	/* if thats too much for you, rip out the repective " -o -path </VFS_dir> -prune" parts */
	shellcode = "find / -path /dev -prune -o -path /proc -prune -o -path /home -prune -o -path /var -prune -o -path /tmp -prune -o -path /sys -prune -o -type f -exec md5sum \\{\\} \\; 2>/dev/null >> /tmp/lsat1.lsat";
    	}
    }
    if ((dostuff(tempfile, 0, shellcode, 0, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.\n");
        return(-1);
    }
    
    /* ok, now all of these find outputs are in the tempfile */
    /* now we print that out to another .out file...         */
  
    if (sumtype == 0)
    {
        /* the sumtype is md5sum */
    	outfile2 = "lsatmd5.out";
    }
    if (sumtype == 1)
    {
        /* the sumtype is sha512 */
	outfile2 = "lsatsha512.out";
	outputfile = "lsatsha512.out";
    }
    /* check if output exists, if it does, make a       */
    /* backup and rm the orig. named one.               */
    tempptr = fopen(outputfile, "r");
    if (tempptr != 0)
    {
        /* output file exists */
	/* make note of this in case we wish to diff... */
	outexists = 1;
        if ((newname = (char *) calloc(2, sizeof(outputfile))) == NULL)
        {
            perror("Can not allocate temp memory in checkmd5.\n");
            perror("Strange. Bailing...\n");
            exit(-1);
        }
        fclose(tempptr);
        /* add .old onto the old outfile name */
        sprintf(newname, "%s.old", outputfile);

        /* gnu, but should be ok... */
        /* no failure check? eh, its ok...   */
        rename(outputfile, newname);

        /* free up calloc'd mem */
        free(newname);
    }

    /* now that we have a backup, remove the outfile */
    /* don't care about failure here...              */
    remove(outputfile);

    /* ok, now that we have a backup of the orig file*/
    /* write the new one out to disk...		     */

    if ((dostuff(tempfile, outfile2, 0, 0, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.\n");
        return(-1);
    }

    /* if the user wants a diff, do it here... */
    /* do they want a diff? */
    if (diff == 1)
    {
	/* make sure the old outfile exists */
	if (outexists == 1)
	{

	    /* note we just overwrite the old diff... */
	    /* this could be improved :)	     */
	    /* note we have two filenames.. md5 and sum512 */
            if (sumtype = 0)
	    {
	    	if (system("diff lsatmd5.out lsatmd5.out.old > lsatmd5.diff") < 0 )
	    	{
			/* something went wrong */
			perror(" diff failed in checkmd5 module.\n");
	    	}
	    	header = "md5 diff completed, check lsatmd5.diff for output\n";
	    	if ((dostuff(0, filename, 0, header, html)) < 0)
            	{
                	/* something went wrong */
        		perror(" Creation of list failed.\n");
        		return(-1);
    	    	}
	    }
	    if (sumtype = 1)
	    {
		/* sha512 */
		if (system("diff lsatsha512.out lsatsha512.out.old > lsatsha512.diff") < 0 )
		{
			/* something went wrong */
			perror(" diff failed in checkmd5 module.\n");
		}
		header = "sha512sum diff completed, check lsatsha512.diff for output\n";
		if ((dostuff(0, filename, 0, header, html)) < 0)
                {
                        /* something went wrong */
                        perror(" Creation of list failed.\n");
                        return(-1);
                }
            }
	}
        else
	{
	    /* old md5sum output file does not exist... */
	    /* user must have del it or not run lsat before. */
	    /* give warning, but don't bomb out...  */	
	    /* and again, check for md5 or sha here */
	    if (sumtype = 0)
	    {
	    	header = "Cound not find lsatmd5.out.old,\nno md5diff run was done; re-run lsat to generate one.\n";
	    }
	    if (sumtype = 1)
	    {
		header = "Could not find lsatsha512.out.old,\nno sha512diff run was done; re-run lsat to generate one\n";
	    }
	    if ((dostuff(0, filename, 0, header, html)) < 0)
    	    {
        	/* something went wrong */
        	perror(" Creation of list failed.\n");
        	return(-1);
    	    }

        }
    }
 
    if (verbose > 0)
    {
        printf(" Finished in checkmd5 module.\n");
    }

    return(0);
}

