/****************************************/
/* This is the chekftpusers module for  */
/* the lsat program. It check the 	*/
/* /etc/ftpusers file and sees what 	*/
/* entries are NOT there that are in	*/
/* /etc/passwd.				*/
/*					*/
/* see lsatmain.c for more details	*/
/****************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int checkftpusers(filename, verbose, html)
const char *const filename;
int verbose;
int html;
{

    char *passptr;		/* ptr to passwd line   */
    char *ftpptr;		/* ptr to ftpusers line */
    /* note we use awk to get only the first entry in */
    /* the passwd list... we ass|u|me that a username */
    /* will be < 120 chars. :O   => line[120]	          */
    char line[120];	/* array for a line	*/
    char temparray[100][120]; /*temparray. */
    char tempstring[100][120];	/* string to hold ftpusername */
    int i=0;		/* counter variable	*/
    int j=0;		/* counter variable  */
    int k=0;		/* counter variable  */
    int inlist=0;		/* flag for strcmp checks */
    int linecount=0;	/* line counter */
    const char * tmp_file = "/tmp/lsat2.lsat";


    FILE *outfile;		/* output file pointer 	*/
    FILE *passfile;		/* file pointer to templist file */
    FILE *ftpfile;		/* file pointer to ftpusers file */
    FILE *fileptr;

    const char * tempfile =NULL;
    const char * shellcode=NULL;
    const char * header   =NULL;

    /* init temparray */
    for (i=0; i<100; i++)
    {
        for (j=0; j<120; j++)
        {
            temparray[i][j]=0;
        }
    }


    /* "you may ask yourself, how do I work this?" */
    /*				Talking Heads  */
    /* This program will read the /etc/passwd file */
    /* and store the username in an array...       */
    /* It will then read the /etc/ftpusers file    */
    /* and see what entries are NOT in ftpusers.   */

    /* IF /etc/ftpusers does not exitst, dont run */

    if (verbose >= 0)
    {
        printf(" Running checkftpusers module...\n");
    }



    if (verbose > 0)
    {
        printf(" Checking ftpusers file...\n");
    }

    /* check that /etc/ftpusers exists and is readable first */
    if ((system("test -r /etc/ftpusers"))!=0)
    {

        /* Just print the header. Since tempfile and */
	/* shellcode are NULL right now, dostuff will*/
	/* only print the header. 		     */

        /* ftpusers does not exist.             */
        /* this _could_ be ok for the user      */
        /* give a msg and leave the area.       */
	header = "While in checkftpusers, it is noted that your /etc/ftpusers file does not exist, or is not readable.\nThis is OK if you are not root, not running ftp or your ftp daemon does not use /etc/ftpusers.\nPlease check your ftp configuration manually.\n";
	if ((dostuff(0, filename, 0, header, html)) < 0)
	{
	    /* something went wrong */
	    perror(" Creation of list failed.");
	    return(-1);
	}
	return(0);
    }
    else
    {  /* start of else if /etc/ftpusers exists */

    /* ok, lets make one pass through the passwd   	*/
    /* note here we only pull the 1st field, which  */
    /* is the "account" name in /etc/passwd.    	*/

    tempfile = "/tmp/lsat1.lsat";
    shellcode = "cat /etc/passwd |awk -F\":\" 'length($1) > 0 {print $1}' 2>/dev/null >/tmp/lsat1.lsat";
    if ((dostuff(tempfile, 0, shellcode, 0, html)) < 0)
    {
	/* something went wrong */
	perror(" Creation of list failed.");
	return(-1);
    }

    /* open the newly created file */
    passfile=fopen("/tmp/lsat1.lsat", "r");
    if (passfile == NULL)
    {
        perror(" Can't open templist...\n");
        perror(" Strange, we just created that file...\n");
        perror(" Giving up in checkftpusers...\n");
        return(-1);
    }


    /* create another temp file so that we can 	*/
    /* run through that list and put it in there*/
    /* we will then pass that list to dostuff   */
    
    if ((fileptr = fopen(tmp_file, "a")) == NULL)
    {
        perror(" Could not create temporary file...\n");
        perror(" Possible link attack while creating/opening file!\n");
        exit(-1);
    }

    /* Note: IIRC, strtok can not be nested if you */
    /* are going to take another token on each one */
    /* i.e. in checkrpms.c it works since we only  */
    /* strtok once on the outside loop since we 	  */
    /* are guarenteed there is only one line in the*/
    /* file since that is how we created it. That  */
    /* is not the case here....			*/

    /* we therefore only grab one filed from the  	*/
    /* passwd file at a time and print that out.		*/
    /* here we are checking accounts on the system  */

    /* reset counter */
    i = 0;
    while (fgets(line, sizeof(line), passfile) != NULL)
    {
        passptr = strtok(line, ", \n");
        /* if not while. we guarentee not more than */
        /* one entry per line with the cat - awk    */
        if (passptr != NULL)
        {
            if (sscanf(passptr, "%119s", temparray[i]) < 1)
            {
                perror(" sscanf failed.\n");
                perror(" Failure was in module: checkftpusers.\n");
                return(-1);
            }
            passptr = strtok(NULL, ", \n");
            if (passptr == NULL)
            {
                ;
            }
        } /* end if (passptr != NULL) */
        /* inc the counter */
        i++;
	if (i>100)
	{
	    perror("Error in module checkftpusers: Too much data.\n");
	    return(-1);
	}
    } /* end while (fgets...) */


    /* close the passfile */
    fclose(passfile);
    /* rm it, no longer needed */
    remove(tempfile);

    /* ok, we have all (we hope) entries from */
    /* /etc/passwd. Now read in /etc/ftpusers */
    /* and compare... I think just one at a   */
    /* time, 'non?				  */


    /* open /etc/ftpusers for read-only */
    ftpfile = fopen("/etc/ftpusers", "r");
    if (ftpfile == NULL)
    {
        perror(" Can't open ftpusers!\n");
        perror(" Check perms or check if file exists.\n");
        perror(" Recall this file should exist. :)\n");
        perror(" Giving up in checkftpusers...\n");
        return(-1);
    }

    /* in the temparray. we just need   */
    /* to compare that to the entries.. */

    /* reset counter */
    j = 0;
    /* get a line from the ftpusers file */
    while (fgets(line, sizeof(line), ftpfile) != NULL)
    {
        ftpptr = strtok(line, ", \n");
        /* if not while. ftpusers = 1 entry per line */
        if (ftpptr != NULL)
        {
            if (sscanf(ftpptr, "%119s", tempstring[linecount]) < 1)
            {
                perror(" sscanf failed.\n");
                perror(" Failure was in module: checkftpusers.\n");
                return(-1);
            }
            ftpptr = strtok(NULL, ", \n");
            if (ftpptr == NULL)
            {
                ;
            }
        } /* end if (ftpptr != NULL) */
        /* inc the linec ounter */
        linecount++;
	if (linecount>100)
	{
	    perror("Error in checkftpusers: Too much data.");
	    return(-1);
	}
    } /* end while (fgets...) */


    /* ok, now compare an entry in the userlist */
    /* we have against the entries in ftpusers  */
    i = 100;
    for (j = 0; j < i; j++)
    {
        for (k=0; k < linecount; k++)
        {
            if ((strcmp(temparray[j], tempstring[k])) == 0)
            {
                /* match */
                /* set flag that it is in list */
                inlist = 1;

            }
        }
        /* flag was set on run? */
        /* no? then that entry was not in ftpusers list */
        if (inlist == 0)
        {
            fprintf(fileptr, "%s\n", temparray[j]);
        }
        /* reset flag */
        inlist = 0;
    }
    /* close the tmpfile */
    fclose(fileptr);

    header = "These accounts are NOT in /etc/ftpusers.\nEnsure that these accounts are in /etc/ftpusers\nor that they _really_ do not need to be restricted.\n";
    if ((dostuff(tmp_file, filename, 0, header, html)) < 0)
    {
        /* something went wrong */
        perror(" Creation of list failed.");
        return(-1);
    } 
    
    /* clean up... */
    fclose(ftpfile);

    /* end of else at the beginning if /etc/ftpusers exists */
    }
    if (verbose > 0)
    {
        printf(" Finished in checkftpusers module.\n");
    }
    return(0);
}
