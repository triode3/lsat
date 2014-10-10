/****************************************/
/* This is the checkpasswd module for   */
/* the lsatmain.c program. It checks    */
/* the /etc/passwd file for entries...  */
/* makes sure only root is setuid 0     */
/* checks for accounts that should be   */
/* terminated.                          */
/*					*/
/* under linux also runs pwck and grpck */
/* to check passwd and groups for 	*/
/* entries that can be deleted... 	*/
/*					*/
/* see lsatmain.c for more details	*/
/****************************************/

#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


int checkpasswd(filename, distribution,  verbose, html)
const char *const filename;
int distribution;
int verbose;
int html;
{

    char *passptr;		/* ptr to passwd file   */
    /* note we use awk to get only the first entry in */
    /* the passwd list... we ass|u|me that a username */
    /* will be < 40 chars. :O   => line[40]	          */
    char line[40];	/* array for a line	*/
    char temparray[2][128]; /*temparray. */
    int j=0;				/* counter variable  */
    int field;			/* field counter	*/
    int fileval; 		/* return val */
    const char * tmp_file = "/tmp/lsat2.lsat"; /* temp file for storage */

    const char * tempfile =NULL;
    const char * shellcode=NULL;
    const char * header   =NULL;

    /* NOT a comprehensive list yet.	*/
    /* just making sure this works...	*/
    char acctlist[13][10] = {"cyrus", "lp", "uucp", "nuucp", "smtp", "listen", "man", "news", "noacces", "nobody4", "nut", "sync", "vpopmail"};
    /* num of entries in above list */
    int acctlistnum = 13;

    FILE *passfile;		/* file pointer to templist file */
    FILE *fileptr;

    if (verbose >= 0)
    {
        printf(" Running checkpasswd module...\n");
    }

    if (verbose > 0)
    {
        printf(" Checking passwd file...\n");
    }


    /* ok, lets make one pass through the passwd   	*/
    /* note here we only pull the 1st field, which  */
    /* is the "account" name in /etc/passwd.    	*/

    shellcode = "cat /etc/passwd |awk -F\":\" 'length($1) > 0 {print $1}' |grep -v \\# 1>/dev/null 2>/dev/null >/tmp/lsat1.lsat";
    tempfile = "/tmp/lsat1.lsat";
    if ((dostuff(tempfile, 0, shellcode, 0, html)) < 0)
    {
        perror(" Creation of list failed.");
        return(-1);
    }

    /* open the newly created file */
    passfile=fopen("/tmp/lsat1.lsat", "r");
    if (passfile == NULL)
    {
        perror(" Can't open templist...\n");
        perror(" Strange, we just created that file...\n");
        perror(" Giving up in checkpasswd...\n");
        return(-1);
    }

    /* create a temp file for storage */
    if ((fileptr = fopen(tmp_file, "a")) == NULL)
    {
        perror(" Could not open temp file...\n");
        perror(" Possible link attack while creating/opening file!\n");
        perror(" Bailing in checkpasswd...\n");
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

    if (verbose > 0)
    {
	printf(" Parsing passwd entries.\n");
    }
    while (fgets(line, sizeof(line), passfile) != NULL)
    {
        field = 0;
        passptr = strtok(line, ", \n");
        /* if not while. we guarentee not more than */
        /* one entry per line with the cat - awk    */
        /* denote also we assume that an account    */
        /* name is no more than 127 chars...        */
        if (passptr != NULL)
        {
            if (sscanf(passptr, "%127s", temparray[1]) < 1)
            {
                perror(" sscanf failed.\n");
                perror(" Failure was in module: checkpasswd.\n");
                return(-1);
            }
            /* check that field against the account list */
            for (j = 0; j < acctlistnum; j++)
            {
                if ((strcmp(temparray[1], acctlist[j])) == 0)
                { 
                    /* match */
                    fprintf(fileptr,"%s\n", temparray[1]);
                }
            }

            passptr = strtok(NULL, ", \n");
            if (passptr == NULL)
            {
                ;
            }
        } /* end while (passptr != NULL) */
    } /* end while (fgets...) */

    /* close input file */
    fclose(passfile);
    /* rm the tempfile, dostuff will use other one */
    remove(tempfile);
    /* close the tmpfile */
    fclose(fileptr);
    close(fileval);
    /* note dostuff below will rm tmpfile */
   
    header = "Please consider removing these system accounts.\nCheck to see if you need them for your system applications before removing.\nAlso, consult the securitylinks.txt file for more information.\n";
    if ((dostuff(tmp_file, filename, 0, header, html)) < 0)
    {
	/* something went wrong */
	perror(" Write to outfile failed.");
	return(-1);
    }


    /* check UID field of passwd file... */
    if (verbose > 0)
    {
        printf(" Checking UIDs in passwd file...\n");
    }

    /* Don't forget to change tmpfile back... */
    tempfile = "/tmp/lsat1.lsat";

    header = "The following accounts are UID 0 in /etc/passwd. Only root should be UID 0.\nRemove if needed.\n";
    shellcode = "awk -F: '($3 == 0) {print $1}' /etc/passwd | grep -v root 1>/dev/null 2>/dev/null >/tmp/lsat1.lsat";
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
 	/* something went wrong */
	perror(" Creation of list failed.");
	return(-1);
    }

    /* now we will check for legacy "+" entries */
    header = "Remove the following entries (if any) from the\n respective passwd/group file(s)\n";
    if (distribution != 3)
    {
        /* we are not on solaris... go for it */
        shellcode = "grep ^+: /etc/passwd /etc/shadow /etc/group 1>/dev/null 2>/dev/null >/tmp/lsat1.lsat";
    }
    else
    {
        /* we are on solaris... */
        shellcode = "/usr/xpg4/bin/egrep \\^\\[+:\\] /etc/passwd /etc/shadow /etc/group 1>/dev/null 2>/dev/null >/tmp/lsat1.lsat";
    }
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
        /* something went wrong */
        perror(" Write to outfile failed.");
        return(-1);  
    }

    /* now we check for accounts with empty passwords */
    header = "The following accounts have no/empty passwords\n";
    shellcode = "awk -F: '($2 == \"\") {print $1}' /etc/shadow 1>/dev/null 2>/dev/null >/tmp/lsat1.lsat";
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0) 
    {
        /* something went wrong */
        perror(" Write to outfile failed.");
        return(-1);
    }

    if ((system("test -r /usr/sbin/pwck")) == 0)
    {
        header = "Output of pwck, note non existent directories, etc\n";
	shellcode = "/usr/sbin/pwck -r 2>/dev/null >/tmp/lsat1.lsat";
	if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
	{
	    /* something went wrong */
	    perror(" Write to outfile failed.");
	    return(-1);
	}
    }

    if ((system("test -r /usr/sbin/grpck")) == 0)
    {
        header = "Output of grpck, note groups it think should be deleted.\n";
	shellcode = "/usr/sbin/grpck -r 2>/dev/null >/tmp/lsat1.lsat";
	if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
	{
	    /* something went wrong */
	    perror(" Write to outfile failed.");
	    return(-1);
	}
    }

    if (verbose > 0)
    {
        printf(" Finished in checkpasswd module.\n");
    }
    return(0);
}
