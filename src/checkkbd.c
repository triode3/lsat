/************************************************/
/* This checks to see if the ctrlaltdel, or the */
/* STOP-A sequences are disabled in linux or    */
/* Solaris, respectively. These should be 	*/
/* disabled for security reasons.		*/
/* Where?					*/
/* Linux: /etc/inittab				*/
/* Solaris: /etc/default/kbd			*/
/*						*/
/* See lsatmain.c for more details...		*/
/************************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int checkkbd(release, filename, verbose, html)
char release[50];
const char *const filename;
int verbose;
int html;
{

    char Sunos[4]="Sun";    /* for comparison	   */
    char Linux[4]="Lin";    /* for comparison	   */
    char *sunptr=Sunos;     /* char pointer		   */
    char *lptr=Linux;	/* char pointer 	   */

    const char * header   =NULL;

    if (verbose >= 0)
    {
        printf(" Running checkkbd module...\n");
    }


    /* ok, we also check here if we are on a Solaris 	*/
    /* machine.  					*/

    if (strstr(release, sunptr) != NULL)
    {

        if (verbose > 0)
        {
	    printf(" Checking KEYBOARD_ABORT...\n");
	}
	/* note, I am checking the existance of the line */
	/* and that it is commented out, which is what   */
	/* we DO NOT want. 				 */

        if ((system("grep KEYBOARD_ABORT=disable /etc/default/kbd 2>/dev/null |grep -v \\# 1>/dev/null 2>/dev/null"))== 0)
	{
            header = "Make sure keyboard abort is disabled.\nIt is in /etc/default/kbd\n";
            if ((dostuff(0, filename, 0, header, html)) < 0)
            {
	        /* something went wrong */
 	        perror(" Write to outfile failed.");
	        return(-1);
	    }
	}
    }
    
    /* if not Solaris... must be linux. */
    if (strstr(release, lptr) != NULL)
    {

        if (verbose > 0)
	{
	    printf(" Checking ctrlaltdel...\n");
	}
	/* note here, it is the opposite of SunOS 	*/
	/* in that the line _should_ be commented out	*/
        if ((system("grep ctrlaltdel /etc/inittab 1>/dev/null 2>/dev/null | grep -v \\# 1>/dev/null 2>/dev/null")) == 0)
	{
	    header = "Make sure the ctrlaltdel is commented out.\nIt is in /etc/inittab.\n";
            if ((dostuff(0, filename, 0, header, html)) < 0)
	    {
	        /* something went wrong */
	        perror(" Creation of list failed.");
	        return(-1);
	    }
	}
    }

    if (verbose > 0)
    {
        printf(" Finished module checkkbd.\n");
    }
    return(0);
}
