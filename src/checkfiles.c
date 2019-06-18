/**************************************/
/* This is the checkfiles module for  */
/* the lsatmain.c program. It checks  */
/* that /tmp and /var/tmp have the    */
/* sticky bit set. Checks utmp, wtmp, */
/* motd, mtab for chmod 644. 	      */
/* checks /usr, /var for root 	      */
/* ownership.			      */
/* It also checks for files on the    */
/* system that have no owner or group */
/*                                    */
/* Future tests on dirs should be put */
/* in this file. I am working on it   */
/**************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <fcntl.h>
#include <unistd.h>

int checkfiles(filename, verbose, html)
const char *const filename;
int verbose;
int html;
{
    /* some local vars...		*/
    const char * in_file = "/tmp/lsat1.lsat";
    const char * secondtf = "/tmp/lsat2.lsat";
    FILE *infile;   /* infile/outfile pointers */
    FILE *fileptr;
    char line[128];	 	 /* temp storage for reading a line */
    char chmod[12]="-rw-r--r--"; /* chmod 644 comparison string  */
    char rooted[2]="0";	  /* uid=0 comparison string	*/
    char *ptr;		  /* char ptr for strtok */
    int count = 0;		  /* general counter     */
    int Linecount = 0;	  /* line counter for file */
    int found = 0;		  /* var set if match found */
    int fileval = 0; 	  /* return val */
    /* note we make tempstring the same as line.  */
    /* this _should_ thwart most buffer overruns, */
    /* as we are doing a sizeof in fgets...	      */
    char tempstring[128];	  /* temp string for sscanf */

    /* things needed for "dostuff" */
    const char * tempfile =NULL;
    const char * shellcode=NULL;
    const char * header   =NULL;

    if (verbose >= 0)
    {
        printf(" Running checkfiles module...\n");
    }

    /* make a list of the files */
    tempfile = "/tmp/lsat1.lsat";
    shellcode = "ls -l / /var 2>/dev/null |grep tmp 2>/dev/null |tee >/tmp/lsat1.lsat";
    if ((dostuff(tempfile, 0, shellcode, 0, html)) < 0)
    {
	/* something went wrong */
	perror(" Creation of list failed.");
	return(-1);
    } 
   
    /* open input file for read... */
    infile = fopen("/tmp/lsat1.lsat", "r");
    if (infile == NULL)
    {
        /* rhut-rho */
	printf(" at first fopen...");
        perror(" Can't open the file we just made!?!\n");
        perror(" Giving up, check perms or something...\n");
        return(-1);
    }

    /* open another temp file, to write out files  */
    /* chmod 600 to that one...			   */

    /* create a temp file for storage */
    if ((fileptr = fopen(secondtf, "a")) == NULL)
    {
        perror(" Could not make temp file...\n");
        perror(" Possible link attack while creating/opening file!\n");
        perror(" Bailing in checkfiles...\n");
        exit(-1);
    }
    
    if (verbose > 0)
    {
        printf(" Checking for sticky bits...\n");
    }

    /* get a line from the file... */
    while (fgets(line, sizeof(line), infile) != NULL)
    {
        ptr = strtok(line, ", \n");
        while (ptr != NULL)
        {
            if (sscanf(ptr, "%127s", tempstring) < 1)
            {
                perror(" sscanf bit the dust");
                return(-1);
            }
            if ((count == 0) && (Linecount == 0))
            {
                /* 1st line in file...  */
                /* note this is ls of / |grep tmp */
                if ((tempstring[9] != 't') && (tempstring[9] != 'T'))
                {
                    /* sticky bit NOT set ! */
                    found = 1;
                    fprintf(fileptr, "Sticky bit not set on /tmp.\n");
                    fprintf(fileptr, "Please set sticky bit on /tmp.\n");
                }
            }
            if ((count ==  0) && (Linecount == 1))
            {
                /* 2nd line in file */
                /* this is ls of /var |grep tmp */
                if ((tempstring[9] != 't') && (tempstring[9] != 'T'))
                {
                    /* sticky bit NOT set! */
                    found = 1;
                    fprintf(fileptr, "Sticky bit not set on /var/tmp.\n");
                    fprintf(fileptr, "Please set sticky bit on /var/tmp.\n");
                }
            }

            /* grab another token */
            ptr = strtok(NULL, ", \"\n");
            if (ptr == NULL)
            {
                ;
            }
            /* inc counter...*/
            count++;
        } /* end of while(ptr != NULL), reset the counter */
        count=0;
        Linecount++;
    } /* end of while(fgets...) */

    /* close files */
    close(fileval);
    fclose(infile);
    fclose(fileptr);

    /* ok, we want to write that out to the outfile, */
    /* and we want to delete the orig tempfile, as   */
    /* dostuff will del the new tempfile...          */
    header = "Checks for sticky bits on tmp files\n";

    if ((dostuff(secondtf, filename, 0, header, html)) < 0)
    {
	/* something went wrong */
	perror(" Writing temp file failed.");
	return(-1);
    }

    /* since dostuff will not do this... */
    remove(in_file);
    
    /* second part... check utmp, wtmp, motd, mtab */
    /* for chmod 644. 				   */


    /* the following will be cecked to see if they are  */
    /* chmod 644...				        */

    /* if the system has any acceptable amount of 	*/
    /* ram subsequent calls to find should be fast	*/

    tempfile = "/tmp/lsat1.lsat";
    shellcode = "find /var -name utmp 2>/dev/null |grep -v find |xargs ls -ln 2>/dev/null >>/tmp/lsat1.lsat";
    if ((dostuff(tempfile, 0, shellcode, 0, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.");
        return(-1);
    }

    shellcode = "find /var -name wtmp 2>/dev/null |grep -v find |xargs ls -ln 2>/dev/null >>/tmp/lsat1.lsat";
    if ((dostuff(0, 0, shellcode, 0, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.");
        return(-1);
    }

    shellcode = "find /etc -name motd 2>/dev/null |grep -v find |xargs ls -ln 2>/dev/null >>/tmp/lsat1.lsat";
    if ((dostuff(0, 0, shellcode, 0, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.");
        return(-1);
    }

    shellcode = "find /etc -name mtab 2>/dev/null |grep -v find |xargs ls -ln 2>/dev/null >>/tmp/lsat1.lsat";
    if ((dostuff(0, 0, shellcode, 0, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.");
        return(-1);
    }

    shellcode = "find /var -name syslog\\*.pid 2>/dev/null |grep -v find |xargs ls -ln 2>/dev/null >>/tmp/lsat1.lsat";
    if ((dostuff(0, 0, shellcode, 0, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.");
        return(-1);
    }

    shellcode = "find /boot -name vmlinuz\\* 2>/dev/null |grep -v find |xargs ls -ln 2>/dev/null >>/tmp/lsat1.lsat";
    if ((dostuff(0, 0, shellcode, 0, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.");
        return(-1);
    }


    /* open input file for read... */
    infile = fopen("/tmp/lsat1.lsat", "r");
    if (infile == NULL)
    {
        /* rhut-rho */
        perror(" Can't open the file we just made!?!\n");
        perror(" Giving up, check perms or something...\n");
        perror(" Problem was in module: checkfiles\n");
        return(-1);
    }

    if (verbose > 0)
    {
        printf(" Checking files that should be chmod 644...\n");
    }

    /* create a temp file for storage */
    if ((fileptr = fopen(secondtf, "a"))==NULL)
    {
        perror(" Could not make file w/perms 0600...\n");
        perror(" Possible link attack while creating/opening file!\n");
        perror(" Bailing in checkfiles...\n");
        exit(-1);
    }

    /* get a line from the file... 	*/
    /* reset counters/flags 1st 		*/
    found = 0;
    count = 0;
    while (fgets(line, sizeof(line), infile) != NULL)
    {
        ptr = strtok(line, ", \n");
        while (ptr != NULL)
        {
            if (sscanf(ptr, "%127s", tempstring) < 1)
            {
                perror(" sscanf bit the dust");
                perror(" Dust biting was in checkfiles.\n");
                return(-1);
            }
            /* this counts the fields we are on */
            count++;
            /* now check for chmod 644 		*/
            /* recall 644 is -rw-r--r-- :)	*/
            /* if count == 1 we should be at */
            /* the 1st field...					*/
            if (count == 1)
            {
                if ((strcmp(tempstring, chmod)) != 0)
                {
                    /* not chmod 644 */
                    /* grab the filename, which should be 		*/
                    /* the 9th field on the line and print it	*/
                    /* set flag to do this... */
                    found = 1;
                }
            }
            if ((found == 1) && (count == 9))
            {
                /* we have a non-chmod 644 and we should 	*/
                /* be at the filename in the sscanf...		*/
		fprintf(fileptr, "%s is not chmod 644.\n", tempstring);
                /* reset counters */
                found = 0;
                count = 0;
            }
            /* grab another token */
            ptr = strtok(NULL, ", \"\n");
            if (ptr == NULL)
            {
                ;
            }
        } /* end while (ptr != NULL) */
        /* reset counters just to be sure... */
        found = 0;
        count = 0;
    } /* end while (fgets...) */

    /* print footer */
    fprintf(fileptr, "Check above files for chmod 644.\n");

    /* close input file before rm */
    close(fileval);

    /* close and rm the first tempfile */
    fclose(infile);
    fclose(fileptr);
    remove(in_file);

    /* tell dostuff to print that out to the output file */
    /* I put the header in the secondtmpfile... 	 */
    if ((dostuff(secondtf, filename, 0, 0, html)) < 0)
    {
        /* something went wrong */
        perror(" Writing of tempfile failed.");
        return(-1);
    }

    /* finally, all dirs in the / should be owned by root */
    /* additionally, all files in /usr should be owned by root, and	*/
    /* /var/tmp should be owned by root.			*/

    tempfile = "/tmp/lsat1.lsat";
    shellcode = "ls -ln / |grep -v total 2>/dev/null |tee > /tmp/lsat1.lsat";
    header = "Dirs that should be owned by root\n";
    if ((dostuff(tempfile, 0, shellcode, header, html)) < 0)
    {
        /* something went wrong */
        perror(" Writing of tempfile failed.");
        return(-1);
    }   

    shellcode = "ls -ln /usr |grep -v total 2>/dev/null | tee >> /tmp/lsat1.lsat";
    if (system(shellcode) < 0)
    {
	perror(" Writing to tempfile failed.\n");
	return(-1);
    }

    shellcode = "ls -ln /var |grep -v total 2>/dev/null |tee >> /tmp/lsat1.lsat";
    if (system(shellcode) < 0)
    {
	perror(" Writing to tempfile failed.\n");
	return(-1);
    }

    /* open input file for read... */
    infile = fopen("/tmp/lsat1.lsat", "r");
    if (infile == NULL)
    {
        /* rhut-rho */
        perror("Can't open the file we just made!?!\n");
        perror("Giving up, check perms or something...\n");
        perror("Error was in module: checkfiles.\n");
        return(-1);
    }

    if (verbose > 0)
    {
        printf(" Checking files that should be owned by root...\n");
    }


    /* create a temp file for storage */
    if ((fileptr = fopen(secondtf, "a"))==NULL)
    {
        perror(" Could not make file w/perms 0600...\n");
        perror(" Possible link attack while creating/opening file!\n");
        perror(" Bailing in checkfiles...\n");
        exit(-1);
    }


    /* get a line from the file... 	*/
    /* reset counters/flags 1st 		*/
    found = 0;
    count = 0;
    while (fgets(line, sizeof(line), infile) != NULL)
    {
        ptr = strtok(line, ", \n");
        while (ptr != NULL)
        {
            if (sscanf(ptr, "%127s", tempstring) < 1)
            {
                perror(" sscanf bit the dust");
                perror(" Dust biting was in checkfiles.\n");
                return(-1);
            }
            /* this counts the fields we are on */
            count++;
            if (count == 3)
            {
                if ((strcmp(tempstring, rooted)) != 0)
                {
                    /* file/dir not owned by root!				*/
                    /* grab the filename, which should be 		*/
                    /* the 9th field on the line and print it	*/
                    /* set flag to do this... */
                    found = 1;
                }
            }
            if ((found == 1) && (count == 9))
            {
                /* we have a non-rooted file...we should	*/
                /* be at the filename in the sscanf...		*/
		fprintf(fileptr, "%s is not owned by root.\n", tempstring);
                /* reset counters */
                found = 0;
                count = 0;
            }
            /* grab another token */
            ptr = strtok(NULL, ", \"\n");
            if (ptr == NULL)
            {
                ;
            }
        } /* end while (ptr != NULL) */
        /* reset counters just to be sure... */
        found = 0;
        count = 0;
    } /* end while (fgets...) */

    /* footer */
    fprintf(fileptr, "Check above dirs to ensure root ownership.\n");
    fprintf(fileptr, "****************************************\n");

    /* clean up		*/
    fclose(infile);
    close(fileval);
    fclose(fileptr);
    /* rm 1st tempfile since dostuff will not do this */
    remove(in_file);

    /* tell dostuff to print that out to the output file */
    /* I put the header in the secondtmpfile...          */
    if ((dostuff(secondtf, filename, 0, 0, html)) < 0)
    {
        /* something went wrong */
        perror(" Writing of tempfile failed.");
        return(-1);
    } 

    if (verbose > 0)
    {
	printf(" Finding files with no owner or no group.\n");
    }

    tempfile = "/tmp/lsat1.lsat";
    shellcode = "find / -nouser -o -nogroup 2>/dev/null |grep -v find |tee 2>/dev/null >>/tmp/lsat1.lsat";
    header = "List of files with no user or group:\n";
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.");
        return(-1);
    }


    if (verbose > 0)
    {
        printf(" Finished in checkfiles.\n");
    }

    return(0);
}

