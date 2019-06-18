/****************************************/
/* This is the checkwww module for lsat	*/
/* It is not totally complete at the    */
/* moment, but should check for SSIs and*/
/* CGI bins in the web server...        */
/* also checks that root is not running */
/* the web server			*/
/*					*/
/* see lsatmain.c for more details	*/
/****************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int checkwww(filename, distribution,  verbose, html)
const char *const filename;
int distribution;
int verbose;
int html;
{

    const char * header   =NULL;

    int execfound = 0;


    if (verbose >= 0)
    {
        printf(" Running checkwww module...\n");
    }

    if (verbose > 0)
    {
	printf(" Checking www config files...\n");
    }

    /* since apache is _mostly_ installed in /etc or /usr */
    /* we will look there for the conf files. I suppose on*/
    /* Solaris it could be in /opt. OTOH, it could be in  */
    /* any location, but then a find / would take time    */

    /* once again, Solaris std location grep can't hang   */
    /* with kewl regexes, so we make exceptions...        */

    if (distribution != 3)
    /* we are not on Solaris, go for it... */
    { 

        if (system("find /etc /usr -name commonapache.conf 2>/dev/null |xargs grep \"Options ExecCGI\" 2>/dev/null |grep -Ev ^\\[:space:\\]*\\#  1>/dev/null 2>/dev/null") == 0 )
        {
            execfound = 1;
	    header = "ExecCGIs were found in commonapache.conf.";
            if ((dostuff(0, filename, 0, header, html)) < 0)
	    {
	    /* something went wrong */
	    perror(" Writing to outfile failed.");
	    return(-1);
	    }
        }

        if (system("find /etc /usr -name apache.conf 2>/dev/null |xargs grep \"Options ExecCGI\" >/dev/null |grep -Ev \\[:space:\\]*\\# 1>/dev/null 2>/dev/null") == 0)
        {
	    execfound = 1;
	    header = "ExecCGIs were found in apache.conf.";
            if ((dostuff(0, filename, 0, header, html)) < 0)
	    {
	        /* something went wrong */
	        perror(" Writing to outfile failed.");
	        return(-1);
	    }
        }

        if (system("find /etc /usr -name httpd.conf 2>/dev/null |xargs grep \"Options ExecCGI\" 2>/dev/null |grep -Ev \\[:space:\\]*\\# 1>/dev/null 2>/dev/null") == 0)
        {
	    execfound = 1;
            header = "ExecCGIs were found in httpd.conf.";
       	    if ((dostuff(0, filename, 0, header, html)) < 0)
	    {
                /* something went wrong */
                perror(" Writing to outfile failed.");
                return(-1);
	    }
         }

    /* end of if (distro != 3) */
    }


    if (distribution == 3)
    /* once again, Solaris does not like [:space:] */
    /* hope for the best here...		   */
    {

        if (system("find /etc /usr -name commonapache.conf 2>/dev/null |xargs grep \"Options ExecCGI\" 2>/dev/null |grep -v \\#  1>/dev/null 2>/dev/null") == 0 )
        {
     	    execfound = 1;
	    header = "ExecCGIs were found in commonapache.conf.";
	    if ((dostuff(0, filename, 0, header, html)) < 0)
	    {
                /* something went wrong */
	        perror(" Writing to outfile failed.");
	        return(-1);
	    }
        }

	if (system("find /etc /usr -name apache.conf 2>/dev/null |xargs grep \"Options ExecCGI\" >/dev/null |grep -v \\# 1>/dev/null 2>/dev/null") == 0)
        {
	     execfound = 1;
	     header = "ExecCGIs were found in apache.conf.";
             if ((dostuff(0, filename, 0, header, html)) < 0)
	     {
	         /* something went wrong */
	         perror(" Writing to outfile failed.");
	         return(-1);
	     }
        }

	if (system("find /etc /usr -name httpd.conf 2>/dev/null |xargs grep \"Options ExecCGI\" 2>/dev/null |grep -v \\# 1>/dev/null 2>/dev/null") == 0)
        {
	     execfound = 1;
             header = "ExecCGIs were found in httpd.conf.";
	     if ((dostuff(0, filename, 0, header, html)) < 0)
	     {
                 /* something went wrong */
                 perror(" Writing to outfile failed.");
                 return(-1);
	     }
         }

	/* end of if (distro == 3) */
    }



    if (execfound == 1)
    {
        header = "Please ensure that the ExecCGIs in the apache \nconf files are needed.\nConsider the Options IncludeNoExec directive, or \ngetting rid of all ExecCGI directives.";
        if ((dostuff(0, filename, 0, header, html)) < 0)
        {
            /* something went wrong */
            perror(" Writing to outfile failed.");
            return(-1);
        }
    }
    
    if (execfound == 0)
    {
	header = "No ExecCGIs found. Good.";
        if ((dostuff(0, filename, 0, header, html)) < 0)
        {
            /* something went wrong */
            perror(" Writing to outfile failed.");
            return(-1);
        }
    }

    /* check that root/nobody is not running the  */
    /* web server. Note that we will have to look */
    /* for apache and httpd... note also that this*/
    /* is not going to work under Solaris. I will */
    /* fix that later on...			  */

    /* recall grep -v changes return code...      */

    /* Solaris ps not the same....		  */
    /* Hrm, for some reason this no workie on OSX */
    /* will have to check this out later...       */
    if (distribution == 3)
    /* on Solaris */
    {
       if (system("ps -ef |grep apache |awk -F\" \" 'length($1) > 0 {print $1}' |grep -v root |grep -v nobody 1>/dev/null 2>/dev/null") != 0)
        {
            header = "apache is being run as root or nobody. Look into this.";
	    if ((dostuff(0, filename, 0, header, html)) < 0)
            {
	        /* something went wrong */
	        perror(" Writing to outfile failed.");
	        return(-1);
	    }
        }
        if (system("ps -ef |grep httpd |awk -F\" \" 'length($1) > 0 {print $1}' |grep -v root |grep -v nobody 1>/dev/null 2>/dev/null") != 0)
	{
            header = "httpd is being run as root or nobody. Look into this.";
            if ((dostuff(0, filename, 0, header, html)) < 0)
            {
            	/* something went wrong */
		perror(" Writing to outfile failed.");
		return(-1);
            }
	}
    }
    else if (distribution == 6)
    { 
        /* slackware */
        if ((system("pgrep -u root httpd||apache 1>/dev/null 2>/dev/null") == 0) || (system("pgrep -u nobody httpd||apache 1>/dev/null 2>/dev/null") == 0))  
        {
            header = "apache is being run as root or nobody. Change this.";
            if ((dostuff(0, filename, 0, header, html)) < 0)
            {
                /* something went wrong */
                perror(" Writing to outfile failed.");
                return(-1);
            }
        }
    }
    else
    {
        /* assume Linux grep */
        if (system("ps aux |grep -E \"apache|httpd\" |awk -F\" \" 'length($1) > 0 {print $1}' |grep  root |grep  nobody 1>/dev/null 2>/dev/null") == 0)
        {
            header = "apache is being run as root or nobody. Change this.";
            if ((dostuff(0, filename, 0, header, html)) < 0)
            {
                /* something went wrong */
                perror(" Writing to outfile failed.");
                return(-1);
            }
        }
    }

    if (verbose > 0)
    {
	printf(" Finished in checkwww module.\n");
    }
   
    return(0);
}

