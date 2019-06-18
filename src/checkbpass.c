/****************************************/
/* This is the checkbpass module ...it  */
/* checks for the presence of the 	*/
/* password line in the grub or lilo 	*/
/* configuration file.			*/
/*					*/
/* See lsatmain.c for more details.	*/
/****************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int checkbpass(distribution, filename, verbose, html)
int distribution;
const char *const filename;
int verbose;
int html;
{
    const char * header   =NULL;

    if (verbose >= 0)
    {
    	printf(" Running checkbpass module...\n");
    }

	/* fsck all this previous noise that I used to do of manually testing for each location. */
	/* find is perfectly capable of looking for us..										 */

	/* first we check for grub, then we will check for lilo		*/
	if ((system("find /boot /etc -name grub.cfg -or -name grub.conf -or -name menu.lst 2>/dev/null >/dev/null")) == 0)
	{
		/* we think we have grub here, lets do grub */
    	if ((system("find /boot /etc -name grub.cfg -or -name grub.conf -or -name menu.lst |xargs grep -i -q password 2>/dev/null >/dev/null")) == 0)
    	{
	    	header = "Password keyword is in grub configuration file, good.\n"; 
		}
		else 
		{
	    	header = "Password keyword is not in grub configuration file, please check.\n";
 		}
	}
	else 
        if ((system("test -r /etc/lilo.conf")) == 0)
        {
            if ((system("grep -i -q password /etc/lilo.conf 2>/dev/null")) == 0)
            {
                    header = "Password keyword found in lilo.conf, good.\n";
            }
            else
            {
                    header = "Password keyword is not in lilo.conf, please check.\n";
            }
        }
        else
        {
            header = "Can not locate grub or lilo configuration files.\nPlease check that the password keyword is being used in them.\n";
        }
    
    /* ok, call dostuff to print out the header */
    /* to the output file... 			*/
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* something went wrong */
	perror(" Creation of list failed.");
	return (-1);
    }
    
    if (verbose > 0)
    {
        printf(" Finished in checkbpass module.\n");
    }

    return(0);
}
