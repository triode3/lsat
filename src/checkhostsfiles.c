/****************************************/
/* This is the checkhostsfiles.c module */
/* for the lsat program. This module    */
/* reads the /etc/hosts.allow and       */
/* /etc/hosts.deny files.		*/
/* It checks for the existance of a 	*/
/* ALL:ALL line in deny, since we think */
/* that a good security policy would be */
/* only admit services we want and deny */
/* all others.				*/
/* It also checks for the existance of  */
/* /etc/hosts.equiv... which is bad     */
/* Also checks for hosts.lpd, and again */
/* we (generally) consider this bad...  */
/*					*/
/* See lsatmain.c for more details	*/
/****************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int checkhostsfiles(filename, verbose, html)
const char *const filename;
int verbose;
int html;
{

    int firstall=0; /* counter for target matches		*/
    int found=0;	/* found for target ALL:ALL		*/
    int found2=0;       /* found for target ALL */
    int field;	/* which field in a line we are on	*/
    int linecount;	/* line counter				*/
    char *ptr; 	/* plain ol' char pointer		*/
    char line[128];  /* for reading in a line from a file 	*/
    char tempstring[128]; /* temp char string		*/
    char target[4]="ALL"; /* what we are looking for 	*/
    FILE *fileptr=NULL;	/* file pointer for input file(s)	*/

    char *strptr = target;

    const char * tempfile =NULL;
    const char * shellcode=NULL;
    const char * header   =NULL;

    if (verbose >= 0)
    {
        printf(" Running checkhostsfiles module...\n");
    }

    if (verbose > 0)
    {
        printf(" Checking list of entries in hosts.allow...\n");
    }
    


    /* don't grep if its not there... we don't want to  */
    /* return an error if its not there, just continue  */
    if ( ((system("cat /etc/hosts.allow 1>/dev/null 2>/dev/null")) > 0) || ((system("grep -v \\# /etc/hosts.allow 1>/dev/null 2>/dev/null")) > 0) )
    {
	/* hosts.allow is not there... */
	header = "hosts.allow not present on this system, or all lines commented out";
	dostuff(0, filename, 0, header, html);
    }
    else /* hosts.allow exists, do all this stuffs */
    {

    /* first off, get the entries in /etc/hosts.allow   */
    /* that are not comments...                         */
    /* note: grep returns
    *  0 on match
    *  1 on no match
    *  2 on system/file/read/other failure
    *  HOWEVER, -v inverts the sense of 0,1.
    */
    tempfile = "/tmp/lsat1.lsat";
    shellcode = "grep -v \\# /etc/hosts.allow |grep -v '^$' 1>/dev/null 2>/dev/null >/tmp/lsat1.lsat";
    if ((dostuff(tempfile, 0, shellcode, 0, html)) < 0)
    {
	/* something went wrong */
	perror(" Creation of list failed.\n");
	return(-1);
    }
    
    /* there should be no "ALL:ALL" in here! */
    /* check for it...			 */

    /* open the file...			 */
    fileptr = fopen("/tmp/lsat1.lsat", "r");
    if (fileptr == NULL)
    {
        perror(" Error opening tmp file.\n");
        perror(" Error was in checkhostsfiles module.\n");
        return(-1);
    }

    /* start getting lines from it		 */
    while (fgets(line, sizeof(line), fileptr) != NULL)
    {
        field=0;
        firstall=0;
        ptr = strtok(line, ", :\n");
        while (ptr != NULL)
        {
            /* note we want to check for:	*/
            /* a) any lines at all		*/
            /* b) ALL:ALL			*/
            /* c) <anything>:ALL		*/
	    /* d) ALL : ALL			*/

            if (sscanf(ptr, "%127s", tempstring) < 1)
            {
                perror(" sscanf bit the dust.\n");
                perror(" Dust biting was in checkhostsfiles.\n");
                return(-1);
            }
            if (field == 0)
            {
                if ((strstr(tempstring, strptr)) != NULL)
                {
                    /* found match, 1st field */
                    firstall = 1;
                }
            }
            if ((field == 1) || (field == 2))
            {
                if (((strstr(tempstring, strptr)) != NULL) &&  (firstall ==1))
                {
                    /* found match, 2nd filed */
		    /* there is an ALL:ALL in hosts.allow */
		    found = 1;
                }
                if (strstr(tempstring, target) != NULL)
                {
                    /* found match, 2nd filed, but not 1st. */
		    /* there is an allow all in hosts.allow */
		    found2 = 1;
                }
            }
            ptr = strtok(NULL, ", :\n");
            if (ptr == NULL)
            {
                /* freak out */
                break;
            }
            field++;
        } /* end while(ptr!=NULL) */
    } /* end while(fgets...) */

    /* we have checked hosts.allow. Close open files */
    fclose(fileptr);
    fileptr=NULL;

    /* note dostuff will rm the tempfile */
    header = "Lines found in hosts.allow\nMake sure you wish to allow the following:\n";
    if ((dostuff(tempfile, filename, 0, header, html)) < 0)
    {
	/* something went wrong */
	perror(" Creation of list failed.");
	return(-1);
    }
   
    } /* end the else of if the hosts.allow exists */

    if (verbose > 0)
    {
        printf(" Checking entries in hosts.deny...\n");
    }


    /* don't grep if its not there... we don't want to  */
    /* return an error if its not there, just continue  */
    if ( ((system("cat /etc/hosts.deny 1>/dev/null 2>/dev/null")) > 0) || ((system("grep -v \\# /etc/hosts.deny 1>/dev/null 2>/dev/null")) > 0) )
    {
        /* hosts.allow is not there... */
        header = "hosts.deny not present on this system, or all lines commented out";
        dostuff(0, filename, 0, header, html);
    }

    else /* hosts.deny exists, do all this stuffs */
    {

    /* now then, get the entries in /etc/hosts.deny         */
    /* that are not comments...                             */
    /* note: grep returns
    *  0 on match
    *  1 on no match
    *  2 on system/file/read/other failure
    *  HOWEVER, -v inverts the sense of 0,1.
    */
    tempfile= "/tmp/lsat1.lsat";
    shellcode = "grep -v \\# /etc/hosts.deny |grep -v '^$' 1>/dev/null 2>/dev/null >/tmp/lsat1.lsat";
    if ((dostuff(tempfile, 0, shellcode, 0, html)) < 0)
    {
	/* something went wrong */
	perror(" Creation of list failed.");
	return(-1);
    }

    /* two things to check for:		*/
    /* a) this file should not be empty	*/
    /* b) it should contain ALL:ALL, or ALL : ALL or the like*/

    /* open the tempfile...			*/
    fileptr = fopen("/tmp/lsat1.lsat", "r");
    if (fileptr == NULL)
    {
        perror(" Error opening tmp file.\n");
        perror(" Error was in checkhostsfiles module.\n");
        return(-1);
    }

    /* reset flags... */
    field = 0;
    found = 0;
    found2 = 0;
    /* start getting lines from this file	*/
    linecount=0;
    while (fgets(line, sizeof(line), fileptr) != NULL)
    {
        ptr = strtok(line, ", \n");
        while (ptr != NULL)
        {
            if (sscanf(line, "%127s", tempstring) < 1)
            {
                perror(" sscanf bit the dust.\n");
                perror(" Dust biting was in checkhostsfiles.\n");
                return(-1);
            }
            if (field == 0)
            {
                if ((strstr(tempstring, strptr)) != NULL)
                {
                    /* found match, 1st field */
                    firstall = 1;
                }
            }
            if ((field == 1) || (field == 2))
            {
                if (((strstr(tempstring, strptr)) != NULL) &&  (firstall ==1))
                {
                    /* found match, 2nd filed */
                    /* there is an ALL:ALL in hosts.allow */
                    found = 1;
                }
                if (strstr(tempstring, target) != NULL)
                {
                    /* found match, 2nd filed, but not 1st. */
                    /* there is an allow all in hosts.allow */
                    found2 = 1;
                }
            }
            ptr = strtok(NULL, ", \n");
            if (ptr == NULL)
            {
                break;
            }
	    field++;
        } /* end while (ptr != NULL) */
        linecount++;
    } /* end while(fgets...) */

    /* clean up */
    if(fileptr)
    {
        fclose(fileptr);
    }

    /* if found = 0, there is _no_ ALL:ALL */
    if (found == 0)
    {
 	header = "Did not find ALL:ALL in hosts.deny.\nLines found in hosts.deny:\n";
    }
    if (found == 1)
    {
        header = "ALL:ALL present in hosts.deny. Good.\nHere are the lines in hosts.deny:\n";
    }

    if ((dostuff(tempfile, filename, 0, header, html)) < 0)
    {
	/* something went wrong */
	perror(" Creation of list failed.");
	return(-1);
    }

    } /* end of else, for if hosts.deny exists */

    /* this is bad unless you really use the "r" commands */
    /* which you shouldn't.. :)				  */
    if ((system("cat /etc/hosts.equiv 1>/dev/null 2>/dev/null")) == 0)
    {
        /* hosts.equiv is there... */
	/* we consider this a bad thing. */
        header = "/etc/hosts.equiv found on this system.\nUnless you really need it, remove it.";
        dostuff(0, filename, 0, header, html);
    }

    if ((system("cat /etc/hosts.lpd 1>/dev/null 2>/dev/null")) == 0)
    {
        /* hosts.lpd is there... */
        /* we consider this a bad thing. */
        header = "/etc/hosts.lpd found on this system.\nConsider preventing lpd from listening to the network.\nAt least make sure it contains fully qualified hostnames.";
        dostuff(0, filename, 0, header, html);
    }

    if (verbose > 0)
    {
        printf(" Finished module checkhostsfiles.\n");
    }

    return(0);
}
