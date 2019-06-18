/****************************************/
/* This is the chekssh module for       */
/* the lsat program. It checks the 	*/
/* /etc/ssh/sshd_config file for some   */
/* entries... note this is hard coded   */
/* for RedHat x.x...                    */
/* should be fine on Solaris 8,9, less  */
/* the grep [:space:] stuffs... :O      */
/*					*/
/* see lsatmain.c for more details	*/
/****************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int checkssh(filename, distribution,  verbose, html)
const char *const filename;
int distribution;
int verbose;
int html;
{

    const char * header   =NULL;


    if (verbose >= 0)
    {
        printf(" Running checkssh module...\n");
    }

    if (verbose > 0)
    {
	printf(" Checking ssh config file...\n");
    }

    header = "sshd config file entries\nMake sure these are commented out.";
    if ((dostuff(0, filename, 0, header, html)) < 0)
    {
        /* something went wrong */
        perror(" Writing to outfile failed.");
        return(-1);
    }

    /* Mac OS X is a little different. I should make lsat look */
    /* for the file instead of hardcoding it... hrmph */
    /* note that on MacOSX the actual file is readable by all. */
    /* this is a bad thing. Not sure why that is the default.  */
    if (distribution == 5)
    {
        /* we are on Darwin */
    if (system("grep \"PermitRootLogin yes\" /etc/sshd_config 2>/dev/null |grep -Ev ^\\[:space:\\]*\\#  1>/dev/null 2>/dev/null") == 0 )
    {
        header = "PermitRootLogin yes found in sshd config.";
         if ((dostuff(0, filename, 0, header, html)) < 0)
        {
        /* something went wrong */
        perror(" Writing to outfile failed.");
        return(-1);
        }
    }

    if (system("grep \"X11Forwarding yes\" /etc/sshd_config 2>/dev/null |grep -Ev \\[:space:\\]*\\# 1>/dev/null 2>/dev/null") == 0)
    {
        header = "X11 Forwarding is enabled in ssh config.";
        if ((dostuff(0, filename, 0, header, html)) < 0)
        {
            /* something went wrong */
            perror(" Writing to outfile failed.");
            return(-1);
        }
    }

    if (system("grep \"RhostsAuthentication yes\" /etc/sshd_config 2>/dev/null |grep -Ev \\[:space:\\]*\\# 1>/dev/null 2>/dev/null") == 0)
    {
        header = "RhostsAuthentication is enabled in ssh config.";
        if ((dostuff(0, filename, 0, header, html)) < 0)
            {
            /* something went wrong */
            perror(" Writing to outfile failed.");
            return(-1);
        }
    }

    if (system("grep \"PermitEmptyPasswords yes\" /etc/sshd_config 2>/dev/null |grep -Ev \\[:space:\\]*\\# 1>/dev/null 2>/dev/null") == 0)
    {
        header = "PermitEmptyPasswords is enabled in ssh config.";
        if ((dostuff(0, filename, 0, header, html)) < 0)
        {
             /* something went wrong */
            perror(" Writing to outfile failed.");
            return(-1);
        }
    }

    if (system("grep \"IgnoreRhosts yes\" /etc/sshd_config 2>/dev/null |grep -Ev \\[:space:\\]*\\# 1>/dev/null 2>/dev/null") == 0)
    {
        header = "IgnoreRhosts is not enabled in ssh config.";
        if ((dostuff(0, filename, 0, header, html)) > 0)
        {
            /* something went wrong */
            perror(" Writing to outfile failed.");
            return(-1);
        }
    }
    
    if (system("grep \"StrictModes yes\" /etc/sshd_config 2>/dev/null |grep -Ev \\[:space:\\]*\\# 1>/dev/null 2>/dev/null") == 0)
    {
	header = "StrictModes is disabled in ssh config.";
	if ((dostuff(0, filename, 0, header, html)) > 0)
	{
	    /* something went wrong */
	    perror(" Writing to outfile failed.");
	    return(-1);
	}
    }

    if (system("grep \"MaxAuthTries \" /etc/sshd_config 2>/dev/null |grep -Ev \\[:space:\\]*\\# 1>/dev/null 2>/dev/null") == 0)
    {
        header = "Ensure MaxAuthTries is enabled and set low, like 5 or 3.";
        if ((dostuff(0, filename, 0, header, html)) > 0)
    {
        /* something went wrong */
        perror(" Writing to outfile failed.");
        return(-1);
    }
    }

    if (system("grep \"AllowUsers \" /etc/sshd_config 2>/dev/null |grep -Ev \\[:space:\\]*\\# 1>/dev/null 2>/dev/null") == 0)
    {
        header = "AllowUsers directive not found in sshd config.\nConsider specifying users to allow by using it.";
        if ((dostuff(0, filename, 0, header, html)) > 0)
    {
        /* something went wrong */
        perror(" Writing to outfile failed.");
        return(-1);
    }
    }

    if (system("grep \"AllowGroups \" /etc/sshd_config 2>/dev/null |grep -Ev \\[:space:\\]*\\# 1>/dev/null 2>/dev/null") == 0)
    {
        header = "AllowGroups directive not found in sshd config.\nConsider specifying groups to allow by using it.";
        if ((dostuff(0, filename, 0, header, html)) > 0)
    {
        /* something went wrong */
        perror(" Writing to outfile failed.");
        return(-1);
    }
    }


    /* end of if (distro != 5) */
    }


    /* once again, I must check distro, as Solaris standard */
    /* grep does not accept [:space:] as a regex...         */

    if ((distribution != 3) && (distribution != 5))
    /* we are not on Solaris, go for it... */
    {

    if (system("grep \"Protocol 2\" /etc/ssh/sshd_config 2>/dev/null |grep -Ev ^\\[:space:\\]*\\# 1>/dev/null 2>/dev/null") != 0)
    {
        header = "Protcol 2 not found in sshd config, or you are doing 1,2.\nChange to protcol 2 only.";
        if ((dostuff(0, filename, 0, header, html)) < 0)
        {
            /* something went wrong */                                                      perror(" Writing to outfile failed.");                                          return(-1);
        }
    }
    if (system("grep \"PermitRootLogin yes\" /etc/ssh/sshd_config 2>/dev/null |grep -Ev ^\\[:space:\\]*\\#  1>/dev/null 2>/dev/null") == 0 )
    {
	header = "PermitRootLogin yes found in sshd config.";
        if ((dostuff(0, filename, 0, header, html)) < 0)
	{
	    /* something went wrong */
	    perror(" Writing to outfile failed.");
	    return(-1);
	}
    }

    if (system("grep \"X11Forwarding yes\" /etc/ssh/sshd_config 2>/dev/null |grep -Ev \\[:space:\\]*\\# 1>/dev/null 2>/dev/null") == 0)
    {
	header = "X11 Forwarding is enabled in ssh config.";
        if ((dostuff(0, filename, 0, header, html)) < 0)
	{
	    /* something went wrong */
	    perror(" Writing to outfile failed.");
	    return(-1);
	}
    }

    if (system("grep \"RhostsAuthentication yes\" /etc/ssh/sshd_config 2>/dev/null |grep -Ev \\[:space:\\]*\\# 1>/dev/null 2>/dev/null") == 0)
    {
	header = "RhostsAuthentication is enabled in ssh config.";
       	if ((dostuff(0, filename, 0, header, html)) < 0)
	    {
            /* something went wrong */
            perror(" Writing to outfile failed.");
            return(-1);
	}
    }

    if (system("grep \"PermitEmptyPasswords yes\" /etc/ssh/sshd_config 2>/dev/null |grep -Ev \\[:space:\\]*\\# 1>/dev/null 2>/dev/null") == 0)
    {
	header = "PermitEmptyPasswords is enabled in ssh config.";
	if ((dostuff(0, filename, 0, header, html)) < 0)
	{
	     /* something went wrong */
            perror(" Writing to outfile failed.");
            return(-1);
    }
    }

    if (system("grep \"IgnoreRhosts yes\" /etc/ssh/sshd_config 2>/dev/null |grep -Ev \\[:space:\\]*\\# 1>/dev/null 2>/dev/null") == 0)
    {
	header = "IgnoreRhosts is not enabled in ssh config.";
	if ((dostuff(0, filename, 0, header, html)) > 0)
	{
	    /* something went wrong */
	    perror(" Writing to outfile failed.");
	    return(-1);
	}
    }

    if (system("grep \"StrictModes yes\" /etc/sshd_config 2>/dev/null |grep -Ev \\[:space:\\]*\\# 1>/dev/null 2>/dev/null") == 0)   
    {
        header = "StrictModes is disabled in ssh config.";
        if ((dostuff(0, filename, 0, header, html)) > 0)    
	{
	    /* something went wrong */  
	    perror(" Writing to outfile failed.");  
	    return(-1);   
	}
    }
 
    if (system("grep \"MaxAuthTries \" /etc/sshd_config 2>/dev/null |grep -Ev \\[:space:\\]*\\# 1>/dev/null 2>/dev/null") == 0)
    {
        header = "Ensure MaxAuthTries is enabled and set low, like 5 or 3.";
        if ((dostuff(0, filename, 0, header, html)) > 0)
    {
        /* something went wrong */
        perror(" Writing to outfile failed.");
        return(-1);
    }
    }

    if (system("grep \"AllowUsers \" /etc/sshd_config 2>/dev/null |grep -Ev \\[:space:\\]*\\# 1>/dev/null 2>/dev/null") == 0)
    {
        header = "AllowUsers directive not found in sshd config.\nConsider specifying users to allow by using it.";
        if ((dostuff(0, filename, 0, header, html)) > 0)
    {
        /* something went wrong */
        perror(" Writing to outfile failed.");
        return(-1);
    }
    }

    if (system("grep \"AllowGroups \" /etc/sshd_config 2>/dev/null |grep -Ev \\[:space:\\]*\\# 1>/dev/null 2>/dev/null") == 0)
    {
        header = "AllowGroups directive not found in sshd config.\nConsider specifying groups to allow by using it.";
        if ((dostuff(0, filename, 0, header, html)) > 0)
    {
        /* something went wrong */
        perror(" Writing to outfile failed.");
        return(-1);
    }
    }


    /* end of if (distro != 3) || (distro != 5) */
    }

    if (distribution == 3)
	/* again, either I suck, or Solaris grep sucks, */
	/* but I can not get Solaris grep or egrep to    */
	/* do [:space:]... hope for the best here...    */
    {
    if (system("grep \"Protcol 2\" /etc/ssh/sshd_config 2>/dev/null |grep -v \\# 1>/dev/null 2>/dev/null") != 0)
    {
        header = "Protcol 2 not found in sshd config, or you are doing 1,2.\nChange to protcol 2 only.";
        if ((dostuff(0, filename, 0, header, html)) < 0)
        {
             /* something went wrong */
            perror(" Writing to outfile failed.");
            return(-1);
        }
    }


    if (system("grep \"PermitRootLogin yes\" /etc/ssh/sshd_config 2>/dev/null |grep -v \\# 1>/dev/null 2>/dev/null") == 0 )
    {
	header = "PermitRootLogin yes found in sshd config.";
        if ((dostuff(0, filename, 0, header, html)) < 0)
	{
            /* something went wrong */
	    perror(" Writing to outfile failed.");
	    return(-1);
	}
    }

    if (system("grep \"X11Forwarding yes\" /etc/ssh/sshd_config 2>/dev/null |grep -v \\# 1>/dev/null 2>/dev/null") == 0)
    {
	header = "X11 Forwarding is enabled in ssh config.";
        if ((dostuff(0, filename, 0, header, html)) < 0)
	{
	/* something went wrong */
	perror(" Writing to outfile failed.");
	return(-1);
        }
    }

    if (system("grep \"RhostsAuthentication yes\" /etc/ssh/sshd_config 2>/dev/null |grep -v \\# 1>/dev/null 2>/dev/null") == 0)
    {
	header = "RhostsAuthentication is enabled in ssh config.";
        if ((dostuff(0, filename, 0, header, html)) < 0)
            {
            /* something went wrong */
            perror(" Writing to outfile failed.");
            return(-1);
	}
    }

    if (system("grep \"PermitEmptyPasswords yes\" /etc/ssh/sshd_config 2>/dev/null |grep -v \\# 1>/dev/null 2>/dev/null") == 0)
    {
	header = "PermitEmptyPasswords is enabled in ssh config.";
	 if ((dostuff(0, filename, 0, header, html)) < 0)
        {
	/* something went wrong */
            perror(" Writing to outfile failed.");
            return(-1);
	}
    }
    if (system("grep \"IgnoreRhosts yes\" /etc/ssh/sshd_config 2>/dev/null |grep  \\# 1>/dev/null 2>/dev/null") == 0)
    {
	 header = "IgnoreRhosts is not enabled in ssh config.";
	if ((dostuff(0, filename, 0, header, html)) > 0)
        {
	    /* something went wrong */
	    perror(" Writing to outfile failed.");
	    return(-1);
	 }
    }
    if (system("grep \"StrictModes yes\" /etc/sshd_config 2>/dev/null |grep \\# 1>/dev/null 2>/dev/null") == 0)
    {
	 header = "StrictModes is disabled in ssh config.";
	 if ((dostuff(0, filename, 0, header, html)) > 0)    
         {
             /* something went wrong */          
	     perror(" Writing to outfile failed.");           
	     return(-1); 
	 }
    }
    if (system("grep \"MaxAuthTries \" /etc/sshd_config 2>/dev/null |grep -\\# 1>/dev/null 2>/dev/null") == 0)
    {
        header = "Ensure MaxAuthTries is enabled and set low, like 5 or 3.";
        if ((dostuff(0, filename, 0, header, html)) > 0)
    {
        /* something went wrong */
        perror(" Writing to outfile failed.");
        return(-1);
    }
    }

    if (system("grep \"AllowUsers \" /etc/sshd_config 2>/dev/null |grep -Ev \\# 1>/dev/null 2>/dev/null") == 0)
    {
        header = "AllowUsers directive not found in sshd config.\nConsider specifying users to allow by using it.";
        if ((dostuff(0, filename, 0, header, html)) > 0)
    {
        /* something went wrong */
        perror(" Writing to outfile failed.");
        return(-1);
    }
    }

    if (system("grep \"AllowGroups \" /etc/sshd_config 2>/dev/null |grep -Ev \\# 1>/dev/null 2>/dev/null") == 0)
    {
        header = "AllowGroups directive not found in sshd config.\nConsider specifying groups to allow by using it.";
        if ((dostuff(0, filename, 0, header, html)) > 0)
    {
        /* something went wrong */
        perror(" Writing to outfile failed.");
        return(-1);
    }
    }


		
    
    /* end of if (distro == 3) */
    }

     
    if (verbose > 0)
    {
	printf(" Finished in checkssh module.\n");
    }
   
    return(0);
}


