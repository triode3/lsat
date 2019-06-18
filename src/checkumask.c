/* This is the checkumask module for    */
/* the lsatmain.c program. It checks    */
/* It checks /etc/csh.login and profile */
/* for the umask setting. It makes sure */
/* that is it set at 022, 027 or 077.	*/
/*					*/
/* see lsatmain.c for more details	*/
/****************************************/

#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define utempi  200  /* added tingmy */

int checkumask(filename, verbose, html)
const char *const filename;
int verbose;
int html;
{


    const char * tempfile;
    const char * shellcode;
    const char * header;

    if (verbose >= 0)
    {
        printf(" Running checkumask module...\n");
    }

    /* Note:
	After looking around at gentoo/redhat/etc I have decided
	to forego the check I used to be performing which was quite
	freakish, and just do a grep of all files in /etc and print
	out only the file and the matching umask found in it. The
	output will tell the user to check that file and umask setting.
	*/

/* - I REALLY wanted to have "^[^#]\{1,\}.*umask[ ]\+.\{3,\}.*" ... 
   - using "grep -he umask|grep -v ^#" isn't good either, no filenames...
*/

    tempfile="/tmp/lsat1.lsat";
    shellcode="grep -e umask /etc/* 2>/dev/null|grep -v bash_completion |grep -v fstab |grep -v :# 2>/dev/null |grep -v directory 2>/dev/null |grep -v if 2>/dev/null|awk -F\"umask\" 'length($1 $2) > 0 {print($1 $2)}' |grep -v octal  >/tmp/lsat1.lsat";
    header="Checking default umask on system:\n\nDefault umask should be 022, 027 or 077. 002 is ok for RedHat.\nHere are the filenames, and the umask number\nfound in each. Please read through the file and ensure that is what you want.\n";
    if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
    {
	/* something went wrong */
	perror(" Could not write to outfile.");
	return(-1);
    }    

    if (verbose > 0)
    {
        printf(" Finished in checkumask module.\n");
    }
    return(0);
}
