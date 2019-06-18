/************************************************************************/
/* This is the main file for lsat					*/
/* It contains the main program and also does two checks of its own.    */
/* It gives the user some usage info (perhaps) and will check the 	*/
/* kernel version and release level of software running on the box.	*/
/*									*/
/* Current release is 0.9.0.						*/
/* Project started on 04/01/2002					*/
/* homepage: http://www.dimlight.org/lsat 			        */
/* changelog: http://www.dimlight.org/lsat/changelog.html  	        */
/*									*/
/* This software Licensed under the GNU General Public License		*/
/* see the bottom of this file for details.			        */
/*									*/
/* This program is _supposed_ to be modular in nature. That is to say:  */
/* all security checks are sub-functions called from this main prog.    */
/*									*/
/************************************************************************/

/****************/
/* Includes	*/
/****************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <fcntl.h>
#include "lsatheader.h"
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>

/********************************************************/
/* Usage function. Tell user command line options	*/
/********************************************************/
void usage()
{
    fprintf(stderr, "Usage is lsat [options]\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t-d\t\t\tdiff current and old md5, output in lsatmd5.diff\n");
    fprintf(stderr, "\t-m <distribution>\tForce a specific distribution test.\n");
    fprintf(stderr, "\t\t\t\tDistro names are: \n");
    fprintf(stderr, "\t\t\t\tdebian\n");
    fprintf(stderr, "\t\t\t\tgentoo\n");
    fprintf(stderr, "\t\t\t\tlinuxmint\n");
    fprintf(stderr, "\t\t\t\tmandrake\n");
    fprintf(stderr, "\t\t\t\tmacosx\n");
    fprintf(stderr, "\t\t\t\tredhat\n");
    fprintf(stderr, "\t\t\t\tslackware\n");
    fprintf(stderr, "\t\t\t\tsolaris\n");
    fprintf(stderr, "\t\t\t\tubuntu\n");
    fprintf(stderr, "\t\t\t\tIf no -f option, lsat will guess. If lsat can\n");
    fprintf(stderr, "\t\t\t\tnot guess the distribution, the default is redhat.\n");

    fprintf(stderr, "\t-a\t\t\tShow this (advanced) help page\n");
    fprintf(stderr, "\t-o <filename>\t\tOutput file name -- default is lsat.out\n");
    fprintf(stderr, "\t-r\t\t\tCheck rpm integrity -- redhat or mandrake only\n");
    fprintf(stderr, "\t-s\t\t\tSilent mode\n");
    fprintf(stderr, "\t-v\t\t\tVerbose output\n");
    fprintf(stderr, "\t-w\t\t\tOutput file in html format\n");
    fprintf(stderr, "\t-x <filename>\t\teXclude module(s) in filelist from checks...\n");
    fprintf(stderr, "\t\t\t\tmodules listed in filename will be excluded\n\t\t\t\tfrom checks. Valid module names are the module\n\t\t\t\tnames themselves without the check.\n\t\t\t\t(e.g. set not checkset)\n\t\t\t\tsee manpage or README for more info\n");

    exit(8);
}

/********************************************************/
/* findexclude function. See if target is in exclude    */
/* list that was on the command line...			*/
/********************************************************/
int findexclude(char xlisting[100], int *xlist)
{
    FILE *fileptr;
    int i=0;
    char *token;
    char tempstring[26];
    char line[256];
    char thelist[37][12] = { {"pkgs"}, {"rpm"}, {"inetd"}, {"inittab"}, {"logging"}, {"set"}, {"write"}, {"dotfiles"}, {"passwd"}, {"files"}, {"umask"}, {"ftpusers"}, {"rc"}, {"kbd"}, {"limits"}, {"ssh"}, {"open"}, {"issue"}, {"www"}, {"md5"}, {"modules"}, {"securetty"}, {"perms"}, {"net"}, {"forward"}, {"promisc"}, {"listening"}, {"cfg"}, {"bpass"}, {"ipv4"}, {"startx"}, {"ftp"}, {"disk"}, {"services"}, {"hostfiles"}, {"pkgupdate"}, {"passtime"} };

    if ((fileptr = fopen(xlisting, "r"))==NULL)
    {
	perror("Error opening exclude file\n");
	return(-1);
    }
    while (fgets(line, sizeof(line), fileptr) != NULL)
    {
        token = strtok(line, ", \"\n");
        if (token == NULL)
        {
            break;
        }
        /* not blank?? Do some stuff...                 */
        while (token != NULL)
        {
            if (sscanf(token, "%25s", tempstring) < 1)
            {
                perror("sscanf bit the dust");
                perror("Error was in lsatmain.\n");
                return(-1);
            }
	    /* ok, now here we compare                      */
	    /* we go through our array of module names and  */
	    /* see if they match. If they do, the user does */
	    /* not want to run that module, so we put it in */
	    /* another array of ints to look through later. */
	    for (i=0;i<35;i++)
	    {
	        if ((strcmp(thelist[i], tempstring)) == 0)
	        { 
	    	/* we have a match. */
		xlist[i]=1;
		}
	    }
	    token = strtok(NULL, ", \"\n");
	    if (token == NULL)
	    {
	        /* freak out */
	        break;
	    }
        } /* end while (ptr != NULLL) */
    } /* end while (fgets...) */
    /* clean up */
    fclose(fileptr);
    return(0);
}
	

/***************************************************/
/* Check what version and kernel release we are at */
/* Changed: 4/4/2002:				   */
/* we will no longer cat /etc/redhat-release 	   */
/* in main... we will now just grab the 1st field  */
/* from the uname -a output so that it is easier   */
/* (later on too) to figure out what we should do. */
/* We shall still grab the "kernel/release" field  */
/* (field 3) to get the linux kernel or release    */
/* (e.g. Solaris version) that we are on. This (I  */
/* think) will give us greater flexibility in      */
/* porting to other *NIXs like Solaris or AIX.     */
/*						   */
/* Note: release will be field 1, kernel field 3   */
/***************************************************/
int versions(char release[], char kernel[], int verbose)
{
    /* some local vars... */
    FILE *infile;         /* file pointer to input file 	*/
    char *ptr;	      /* char pointer for strtok    	*/
    char line[100];       /* temp array for reading lines 	*/
    /* note make tempstring 100 also, avoid buffer overruns */
    char tempstring[100]; /* temp string for sscanf		*/
    int  linecount=0;     /* line counter			*/
    int  field=0;	      /* field counter for strtok	*/
    int  i=0;	      /* general counter variable	*/
    int  fileval=0;	      /* return value 	*/

    /* First off, get the output of uname -a                        */
    /* this is silly, I should know how to do this more easily      */


    if ((fileval = open("/tmp/lsat1.lsat", O_RDWR | O_CREAT | O_EXCL | O_EXCL, 0600)) < 0)
    {
        perror("Could not make file w/perms 0600...\n");
        perror("Possible link attack while creating/opening file!\n");
        perror("Bailing in lsatmain...\n");
        exit(-1);
    }

    /* get the uname, redirect to tempfile	*/
    if ( (system("uname -a >> /tmp/lsat1.lsat")) < 0)
    {
        /* rhut-rho...something bad happened.                   */
        perror("Problem executing pwd...\n");
	perror("Error was in lsatmain.\n");
        return(0);
    }

    /* open input file for read... */
    if (NULL == (infile = fdopen(fileval, "r")))
    {
        /* rhut-rho */
        perror("Can't open the file we just made!?!\n");
        perror("Giving up, check perms or something...\n");
	perror("Error was in lsatmain.\n");
        return(-1);
    }


    /* get a line from the file... it should contain only 2	*/
    /* 1st line is uname -a, so the third field is the kernel version */
    while (fgets(line, sizeof(line), infile) != NULL)
    {
        /* (re)set field...	*/
        field=0;
        ptr = strtok(line, ", \"\n");
        if (ptr == NULL)
        {
            break;
        }
        /* not blank?? Do some stuff...			*/
        while (ptr != NULL)
        {
            /* if linecount == 0, get 1st field */
            if (sscanf(ptr, "%99s", tempstring) < 1)
            {
                perror("sscanf bit the dust");
		perror("Error was in lsatmain.\n");
                return(-1);
            }
            if ((field == 0) && (linecount == 0))
            {
                for (i = 0; i < 50; i++)
                {
                    release[i]=tempstring[i];
                }
            }
            if ((field == 2) && (linecount == 0))
            {
                for (i = 0; i < 3; i++)
                {
                    kernel[i]=tempstring[i];
                }
            }
            ptr = strtok(NULL, ", \"\n");
            if (ptr == NULL)
            {
                /* freak out */
                break;
            }
            /* increment counter for the field we are reading */
            field++;
        } /* end while(ptr != NULL) */
        /* increment the line counter */
        linecount++;
    } /* end while (fgets...) */
    close(fileval);
    /* clean up even though we will rm it */
    fclose(infile);

    if ( (system("rm -f /tmp/lsat1.lsat")) < 0)
    {
        /* rhut-rho...something bad happened.           */
        perror("Problem rm'ing tempfile...\n");
	perror("Error was in lsatmain.\n");
        return(0);
    }

    if (verbose > 0)
    {
        printf("System type is: %s\n", release);
        printf("Kernel/release level is: %s\n", kernel);
    }

    return(0);
}

/****************************************************************/
/* lsatrm. This function will remove temp files that lsat uses  */
/* should a module bomb out. All temp files should be called    */
/* lsatn.lsat where n is an integer, and should be in /tmp/     */
/****************************************************************/
int lsatrm(void)
{
    if ((system("rm -f /tmp/lsat*.lsat 1>/dev/null 2>/dev/null")) != 0)
    {
	perror("Error removing tmp files.\n");
	perror("This could be a symlink attack.\n");
	return(-1);
    }
    return(0);
}


/****************************************************************/
/* Main <smile> All sub-programs/functions should be called 	*/
/* from here. Note: they should also be listed in lsatheader.h  */
/****************************************************************/
int main(int argc, char *argv[])
{
    int retval=0;     /* a return value indicator		*/
    int verbose=0;    /* for verbosity			*/
    /* note: switches: 
    *  lsat -a = advanced help
    *  lsat -d = perform a diff of the md5output and old md5output
    *  lsat -v = verbose, verbose = 1
    *  lsat    = slightly verbose, verbose = 0, 
    * the module name is printed as it performes each module
    *  lsat -s = silent, verbose = -1, nothing printed at all.
    *  lsat -r = run checkrpm module.
    *  lsat -m = force a distribution mode, e.g. lsat -m redhat
    *  lsat -h = give help output
    *  lsat -o outfile = put output in outfile instead of lsat.out	
    *  lsat -w = print output in html format
    *  lsat -x module = exclude module from checks
    */
    int diff=0; /* do we run the md5 diff? */
    int rpmmodule=0; /* do we run the rpm module? */
    int inetexists=0; /* if inetd.conf exists, this is 1 	*/
    int distribution=1; /* what distro are we on?		*/
    /* -1=user specified a distro, its in man_distro 
    * 1=redhat 
    * 2=debian 
    * 3=solaris 
    * 4=gentoo
    * 5=macosx 
    * 6=slackware
    * 7=ubuntu
    * 8=linuxmint
    * default = redhat   */
    int html=0; /* html output? */
    /* html = 0: normal output
    *  html = 1: user wants html output
    */ 
    char release[50]; /* array for release level		*/
    char kernel[50];  /* what kernel user is running	*/
    static char man_distro[10]; /* if the user specifies a distribution */
    const char * header   =NULL;   /* to print out the header */
    static char out_file[255] = "lsat.out";  /* output filename var  */
    char xlist[100]; /* modules to exclude */
    int xarray[37] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
    int  somethinginxlist = 0;
    /* note if no filename given, default = lsat.out	*/
    FILE *tempptr;    /* a temp file pointer		*/

     uid_t uidme;
     uidme = getuid();

    /* we used to set the umask of the user here... */
    /* note that does not work all of the time.     */
    /* instead use open with options below. Also,   */
    /* warn user at bottom about setting umask to   */
    /* sensible value before modifying files.       */

    /* we will again try to set umask here...       */
    mode_t old_umask;

    old_umask=(umask(0177));


    /* Now do options processing */
    int i;
    for (i=1; i < argc; i++)
    {
	if(argv[i][0] == '-')
	{
	    switch (argv[i][1]) 
	    {
		case 'a': 
			  usage(); 
			  break;
	    case 'h':
			  usage();
			  break;
		case 'd': 
			  diff = 1; 
			  break;
		case 'm': 
			  strncpy(man_distro, argv[i]+3,sizeof(man_distro));
			  break;
		case 'o': 
			  strncpy(out_file, argv[i]+3,sizeof(out_file));
			  break;
		case 'r': 
			  rpmmodule = 1; 
			  break;
		case 's': 
			  verbose = -1; 
			  break;
	    case 'v': 
			  verbose = 1; 
			  break;
		case 'w': 
			  html = 1; 
			  strcpy(out_file, "lsat.html"); 
			  break;
		case 'x': strcpy(xlist,argv[i]+3);
			  somethinginxlist = 1;
			  break;
	    default : 
			  usage(); 
		break;
	    }
	}
    }

    if (!out_file) /* if no filename after -o */
    {
        usage();  /* note usage() will exit lsat */
    }

    /* if an exclude list was given, parse the file and */
    /* fill the array of excludes..			*/
    if (somethinginxlist == 1)
    {
	if (findexclude(xlist, xarray) != 0)
	{
	     /* something went wrong */
	     perror("Error reading exclude list...\n");
	     return(-1);
	}
    }

    /* check if output exists, if it does, make a 	*/
    /* backup and rm the orig. named one.		*/
    tempptr = fopen(out_file, "r");
    if (tempptr != 0)
    {
        /* output file exists */
        /* make a backup copy */
	char *newname = NULL;
	if ((newname = (char *) calloc(2, sizeof(out_file))) == NULL)
	{
	    perror("Can not allocate temp memory in lsatmain.\n");
	    perror("Strange. Bailing...\n");
	    exit(-1);
	}
        fclose(tempptr);
	/* add .old onto the old outfile name */
        sprintf(newname, "%s.old", out_file);

        /* gnu, but should be ok... */
        /* whoops, we are now failure checking this...   */
        retval = 0;
        retval = rename(out_file, newname);
        if (retval < 0)
        {
            /* damn, rename bombed */
            perror("GNU rename bombed in lsatmain.\n");
            perror("Very odd, you should have seen errors, report them :) \n");
            exit(-1);
        }
	
	/* free up calloc'd mem */
	free(newname);
    }
    
    /* now that we have a backup, remove the outfile */
    /* don't care about failure here...              */
    remove(out_file);

    /* not seem to work 100% of the time on my system*/
    /* for making files w/chmod 0600...              */
    /* reverting to open with options...             */
    retval = 0;
    if ((retval = open(out_file, O_RDWR | O_CREAT | O_EXCL, 0600)) < 0) 
    {
	perror("Could not make file w/perms 0600...\n");
	perror("Possible link attack while creating/opening file!\n");
	perror("Bailing in lsatmain...\n");
        exit(-1);
    }

    /* we print this out no matter what verbose is */
    printf("Starting LSAT...\n");

    /* remove old tempfiles (if lsat got killed) */
    /* if this fails, we should stop _now_! */
    if (lsatrm() < 0)
    {
	perror("Could not remove tempfiles during startup.\n");
	perror("You may need root permissions to do this.\n");
	return(-1);
    }
    
    /* read that in, check kernel version and "release"	*/
    if (verbose >= 0)
    {
        printf("Getting system information...\n");
    }

    retval = versions(release, kernel, verbose);
    if (retval < 0)
    {
        /* something went wrong... 			*/
        perror("Problem in versions function.\n");
        return(0);
    }

    /* note about Mandrake: until I can find vast differences */
    /* between RedHat/Mandrake in terms of configs, we assume */
    /* that it is redhat and don't check /etc/mandrake_rel... */
    /* simple check to see if we are on RedHat or Debian */
    /* first, did the user specify a distro? */
    if (distribution != -1)
    {
        if ((system("cat /etc/redhat-release 1>/dev/null 2>/dev/null >/dev/null")) == 0)
        {
            /* we are on redhat */
            distribution = 1;
        }
	/* here we are going to test for LinuxMint then Ubuntu, if those fail we will test for Debian */
	/* since doing this the other way around would lead to two cases being true (in LinuxMint and */
	/* Ubuntu they both still use /etc/devbian_version					      */
	/* Note, for now we -could- assume LinuxMint and Debian are equal, but this may not always be */
	/* true, so I am using different distribution numbers here...				      */
	if ((system("grep -R DISTRIB_ID /etc/*-release |grep Ubuntu 1>/dev/null 2>/dev/null >/dev/null")) == 0)
	{
	    /* we are pretty sure we are on Ubuntu */
	    distribution = 7;
	}
	else if ((system("grep -R /etc/*-release DISTRIB_ID |grep LinuxMint 1>/dev/null 2>/dev/null >/dev/null")) == 0)
	{
	    /* we are pretty sure we are on LinuxMint */
		distribution = 8;
	}
	else if ((system("cat /etc/debian_version 1>/dev/null 2>/dev/null >/dev/null")) == 0)
	{
        /* we are on debian */
        distribution = 2;
	}
	if ((system("uname |grep SunOS 1>/dev/null 2>/dev/null >/dev/null")) == 0)
	{
	    /* we are on Solaris/SunOS */
	    distribution = 3;
	}
	/* I am not 100% sure that the uname -a on gentoo will always present gentoo, hence the check below it */
    if ((system("uname -a |grep gentoo 1>/dev/null 2>/dev/null >/dev/null")) == 0)
    {
	    /* we are on gentoo */
	    distribution = 4;
	} 
	if ((system("grep -R -i /etc/*-release gentoo 1>/dev/null 2>/dev/null >/dev/null")) == 0)
 	{
		/* we are on gentoo */
	    distribution = 4;
	}
    if ((system("uname |grep Darwin 1>/dev/null 2>/dev/null >/dev/null")) == 0)
	{
	    /* we are on Mac OSX */
	    distribution = 5;
	}
    if ((system("cat /etc/slackware-version 1>/dev/null 2>/dev/null >/dev/null")) == 0)  /* JTO */
    {
            /* we are on slack */
            distribution = 6;
    }
    }
    if (distribution == -1)
    {
	/* user specified a distribution */
        if ((strcmp(man_distro, "redhat")) == 0)
	{
	    /* redhat */
	    distribution = 1;
	}
        if ((strcmp(man_distro, "centos")) == 0)
	{
	    /* CentOS, RHEL-alike */
	    distribution = 1;
	}
        if ((strcmp(man_distro, "caos")) == 0)
	{
	    /* CaOS, RHL-alike */
	    distribution = 1;
	}
        if ((strcmp(man_distro, "fedora")) == 0)
	{
	    /* Post-RHL */
	    distribution = 1;
	}
	if ((strcmp(man_distro, "debian")) == 0)
	{
	    /* debian */
	    distribution = 2;
	}
	if ((strcmp(man_distro, "solaris")) == 0)
	{
	    /* solaris */
	    distribution = 3;
	}
	if ((strcmp(man_distro, "mandrake")) == 0)
 	{
	    /* mandrake, same as redhat, sortof */
	    distribution = 1;
	}
	if ((strcmp(man_distro, "gentoo")) == 0)
	{
	    /* gentoo */
	    distribution = 4;
	}
 	if ((strcmp(man_distro, "macosx")) == 0)
	{
	    /* Mac OSX */
	    distribution = 5;
	}
    if ((strcmp(man_distro, "slackware")) == 0) /* JTO */
    {
        /* slackware- JTO */
        distribution = 6;
    }
	if ((strcmp(man_distro, "ubuntu")) == 0)
	{
	    /* ubuntu */
	    distribution = 7;
	}
	if ((strcmp(man_distro, "linuxmint")) == 0)
	{
	    /* linuxmint */
	    distribution = 8;
    }
	/* default is redhat */
    else
	{
	   distribution = 1;
    }
    } /* end if (distribution == -1) */

    /* ********* checks are below ******************  */
    /* ok, we should have enough info, run the checks */

    if (verbose >= 0)
    {
	printf("Running modules...\n");
    }

    /* we need to make a header if html = 1, as this */
    /* could confuse the issue by putting it in the  */
    /* first module... call dostuff here...          */
   
    if (html == 1)
    {
	html = 2; /* this tells dostuff to write out header html */
	header = "\n";  /* can't be NULL */
        if ((dostuff(0, out_file, 0, header, html)) < 0)
        {
            /* something went wrong */
            perror(" Creation of list failed.");
            return (-1);
        }
        html = 1;   /* change html int back to one */
    }
 

    /* run checkpkgs */
    /* did user put it in exclude list? */
    if (xarray[0] == 0 )
    {
        retval = 0;
        retval = checkpkgs(distribution, out_file, verbose, html);
        if (retval < 0)
        {
            /* something went wrong...                      */
            perror("Problem in checkpkgs module.\n");
            perror("You should have seen errors...\n");
            /* module bombed, clean up */
            lsatrm();
            return(0);
        }
    }

    /* run checkrpm if rpmmodule = 1 and distribution =1 */
    /* did user put it in exclude list? */
    if (xarray[1] == 0)
    {
        retval = 0;
        if ((rpmmodule == 1) && (distribution == 1))
        {
            retval = checkrpm(out_file, verbose, html);
            if (retval < 0)
	    {
	        /* something went wrong...                      */
                perror("Problem in checkrpm module.\n");
                perror("You should have seen errors...\n");
                /* module bombed, clean up */
                lsatrm();
                return(0);
	    }
        }
    }

    /* run checkinetd */
    /* did user put it in exclude list? */
    if (xarray[2] == 0)
    {

	retval = 0;
    	retval = checkinetd(out_file,distribution, verbose, html);
    	if (retval < 0)
    	{
	    /* something went wrong...		*/
	    perror("Problem in checkinetd module.\n");
	    perror("You should have seen errrors...\n");
            lsatrm();
	    return(0);
        }
    }

    /* check inittab		*/
    /* did user put it in exclude list? */
    if (xarray[3] == 0)
    {
    	retval = 0;
    	retval = checkinittab(out_file, distribution, verbose, html);
    	if (retval < 0)
    	{
            /* something went wrong...   */
	    perror("Problem in checkinittab module.\n");
	    perror("You should have seen errors...\n");
	    /* module bombed, clean up */
	    lsatrm();
	    return(0);
        }
    }

    /* check logging		*/
    /* did user put it in exclude list? */
    if (xarray[4] == 0)
    {
        retval = 0;
        retval = checklogging(out_file, distribution, verbose, html);
    	if (retval < 0)
    	{
            /* something went wrong...   */
            perror("Problem in checklogging module.\n");
            perror("You should have seen errors...\n");
            /* module bombed, clean up */
            lsatrm();
            return(0);
        }
    }
        

    /* check for setuid/setgid files */
    /* did user put it in exclude list? */
    if (xarray[5] == 0)
    {
        retval = 0;
        retval = checkset(out_file, verbose, html);
        if (retval < 0)
        {
            /* something went wrong...                      */
            perror("Problem in checkset module.\n");
            perror("You should have seen errors...\n");
            /* module bombed, clean up */
            lsatrm();
            return(0);
        }
    }


    /* check for world writable files/directories */
    /* did user put it in exclude list? */
    if (xarray[6] == 0)
    {
        retval = 0;
        retval = checkwrite(out_file, distribution, verbose, html);
        if (retval < 0)
        {
            /* something went wrong...                      */
            perror("Problem in checkwrite module.\n");
            perror("You should have seen errors...\n");
            /* module bombed, clean up */
            lsatrm();
            return(0);
        }
    }

    /* check for .exrc and .forward files */
    /* did user put it in exclude list? */
    if (xarray[7] == 0)
    {
    	retval = 0;
    	retval = checkdotfiles(out_file, verbose, html);
    	if (retval < 0)
        {	
            /* something went wrong...                      */
            perror("Problem in checkdotfiles module.\n");
            perror("You should have seen errors...\n");
            /* module bombed, clean up */
            lsatrm();
            return(0);
        }
    }


    /* check /etc/passwd for uid 0 other than */
    /* root and users not needed on the sys.  */
    /* did user put it in exclude list? */
    if (xarray[8] == 0)
    {
    	retval = 0;
    	retval = checkpasswd(out_file, distribution, verbose, html);
    	if (retval < 0)
    	{
            /* something went wrong...                      */
            perror("Problem in checkpasswd module.\n");
            perror("You should have seen errors...\n");
	   
	    /* module bombed, clean up */
            lsatrm();
            return(0);
        }
    }

    /* check to see if sticky bits are set */
    /* on /tmp & /var/tmp. I will add more */
    /* to this later on...			*/
    /* did user put it in exclude list? */
    if (xarray[9] == 0)
    {
        retval = 0;
        retval = checkfiles(out_file, verbose, html);
        if (retval < 0)
        {
            /* something went wrong...			*/
            perror("Problem in checkfiles module.\n");
            perror("You should have seen errors...\n");
	    /* module bombed, clean up */
            lsatrm();
            return(0);
        }
    }

    /* did user put it in exclude list? */
    if (xarray[10] == 0)
    {
        retval = 0;
        retval = checkumask(out_file, verbose, html);
        if (retval < 0)
        {
            /* something went wrong...                      */
            perror("Problem in chumask module.\n");
            perror("You should have seen errors...\n");
	    /* module bombed, clean up */
            lsatrm();
            return(0);
        }
    }

    /* did user put it in exclude list? */
    if (xarray[11] == 0)
    {
        retval = 0;
        retval = checkftpusers(out_file, verbose, html);
        if (retval < 0)
        {
            /* something went wrong...                      */
            perror("Problem in checkftpusers module.\n");
            perror("You should have seen errors...\n");
	    /* module bombed, clean up */
            lsatrm();
            return(0);
        }
    }

    /* did user put it in exclude list? */
    if (xarray[12] == 0)
    {
        retval = 0;
        retval = checkrc(release, kernel, distribution, out_file, verbose, html);
        if (retval < 0)
        {
            /* something went wrong...			*/
            perror("Problem in checkrc module.\n");
            perror("You should have seen errors...\n");
	    /* module bombed, clean up */
            lsatrm();
            return(0);
        }
    }

    /* did user put it in exclude list? */
    if (xarray[13] == 0)
    {
        retval = 0;
        retval = checkkbd(release, out_file, verbose, html);
        if (retval < 0)
        {
            /* something went wrong...                      */
            perror("Problem in checkkbd module.\n");
            perror("You should have seen errors...\n");
	    /* module bombed, clean up */
            lsatrm();
            return(0);
        }
    }

    /* did user put it in exclude list? */
    if (xarray[14] == 0)
    {
        if (distribution != 6) /* need to fix for slack, JTO */
        {
        retval = 0;
        retval = checklimits(out_file, verbose, html);
        if (retval < 0)
        {
            /* something went wrong...                      */
            perror("Problem in checklimits module.\n");
            perror("You should have seen errors...\n");
            /* module bombed, clean up */
            lsatrm();
            return(0);
        }
        } /* end if its not slack if statement */
    }

    /* did user put it in exclude list? */
    if (xarray[15] == 0)
    {
        retval = 0;
    	retval = checkssh(out_file, distribution, verbose, html);
    	if (retval < 0)
        {
	    /* something went wrong..	*/
	    perror("Problem in checkssh module.\n");
	    perror("You should have seen errors...\n");
	    lsatrm();
	    return(0);
        }
    }

    /* did user put it in exclude list? */
    if (xarray[16] == 0)
    {
        retval = 0;
        retval = checkopenfiles(out_file, distribution, verbose, html);
        if (retval < 0)
        {
            /* something went wrong..       */
            perror("Problem in checkopenfiles module.\n");
            perror("You should have seen errors...\n");
            lsatrm();
            return(0);
        }
    }

    /* did user put it in exclude list? */
    if (xarray[17] == 0)
    {
        retval = 0;
        retval = checkissue(out_file, verbose, html);
        if (retval < 0)
        {
            /* something went wrong..       */
            perror("Problem in checksissue module.\n");
            perror("You should have seen errors...\n");
            lsatrm();
            return(0);
        }
    }

    /* did user put it in exclude list? */
    if (xarray[18] == 0)
    {
        retval = 0;
        retval = checkwww(out_file, distribution, verbose, html);
        if (retval < 0)
        {
            /* something went wrong..       */
            perror("Problem in checkwww module.\n");
            perror("You should have seen errors...\n");
            lsatrm();
            return(0);
        }
    }

    /* did user put it in exclude list? */
    if (xarray[19] == 0)
    {
        retval = 0;
        retval = checkmd5(out_file, distribution, verbose, html, diff);
        if (retval < 0)
        {
            /* something went wrong..       */
            perror("Problem in checkmd5 module.\n");
            perror("You should have seen errors...\n");
            lsatrm();
            return(0);
        }
    }

    /* did user put it in exclude list? */
    if (xarray[20] == 0)
    {
        if  ((distribution != 3) && (distribution !=5))
    	/* we are not on Solaris, so we can run this module */
    	{
            retval = 0;
            retval = checkmodules(out_file, verbose, html);
            if (retval < 0)
            {
                /* something went wrong..       */
                perror("Problem in checkmodules module.\n");
                perror("You should have seen errors...\n");
                lsatrm();
                return(0);
            }
        }
    }

    /* did user put it in exclude list? */
    if (xarray[21] == 0)
    {
        retval = 0;
        retval = checksecuretty(out_file, verbose, html);
        if (retval < 0)
        {
            /* something went wrong..       */
            perror("Problem in checksecuretty module.\n");
            perror("You should have seen errors...\n");
            lsatrm();
            return(0);
        }
    }

    /* did user put it in exclude list? */
    if (xarray[22] == 0)
    {
        retval = 0;
        retval = checkrcperms(out_file, distribution, verbose, html);
        if (retval < 0)
        {
	    /* something went wrong...	*/
	    perror("Problem in checkrcperms module.\n");
	    perror("You should have seen errors...\n");
	    lsatrm();
	    return(0);
        }
    }

    /* did user put it in exclude list? */
    if (xarray[23] == 0)
    {
        retval = 0;
        retval = checknet(out_file, distribution, verbose, html);
        if (retval < 0)
        {
            /* something went wrong...                      */
            perror("Problem in checknet module.\n");
            perror("You should have seen errors...\n");
	    /* module bombed, clean up */
            lsatrm();
            return(0);
        }
    }

    /* did user put it in exclude list? */
    if (xarray[24] == 0)
    {
        retval = 0;
        retval = checknetforward(out_file, distribution, verbose, html);
        if (retval < 0)
        {
            /* something went wrong...                      */
            perror("Problem in checknetforward module.\n");
            perror("You should have seen errors...\n");
            /* module bombed, clean up */
            lsatrm();
            return(0);
        }
    }


    /* did user put it in exclude list? */
    if (xarray[25] == 0)
    {
        if ((distribution !=3) && (distribution != 5))
        {
            /* we are on linux... */
            retval = 0;
            retval = checknetp(kernel, out_file, distribution, verbose, html);
            if (retval < 0)
            {
                perror("Problem in checknetpromisc module.\n");
                perror("You should have seen errors...\n");
                lsatrm();
                return(0);
            }
        }
    }

    /* did user put it in exclude list? */
    if (xarray[26] == 0)
    {
        if (distribution == 1) /* redhat (or derived) specific */
        {
            retval = 0;
            retval = checkcfg(out_file, verbose, html);
            if (retval < 0)
            {
                /* something went wrong...                      */
                perror("Problem in checkcfg module.\n");
                perror("You should have seen errors...\n");
                /* module bombed, clean up */
                lsatrm();
                return(0);
            }
        }
    }

    /* did user put it in exclude list? */
    if (xarray[27] == 0)
    {
        if ((distribution !=3) && (distribution != 5)) /* we are on linux */
        {
            retval = 0;
            retval = checkbpass(distribution, out_file, verbose, html);
            if (retval < 0)
            {
                /* something went wrong...                      */
                perror("Problem in checkbpass module.\n");
                perror("You should have seen errors...\n");
                /* module bombed, clean up */
                lsatrm();
                return(0);
            }
        }
    }

    /* did user put it in exclude list? */
    if (xarray[28] == 0)
    {
        if ((distribution != 3) && (distribution != 5)) /* we are on linux */
        {
            retval = 0;
            retval = checkipv4(out_file, verbose, html);
            if (retval < 0)
            {
                /* something went wrong...                      */
                perror("Problem in checkipv4 module.\n");
                perror("You should have seen errors...\n");
                /* module bombed, clean up */
                lsatrm();
                return(0);
            }
        }
    }

    /* did user put it in exclude list? */
    if (xarray[29] == 0)
    {
        if ((distribution !=3) && (distribution !=5)) /* we are on linux */
        {
            retval = 0;
            retval = checkx(out_file, verbose, html);
            if (retval < 0)
            {
                /* something went wrong...                      */
                perror("Problem in checkx module.\n");
                perror("You should have seen errors...\n");
                /* module bombed, clean up */
                lsatrm();
                return(0);
            }
        }
    }

    /* did user put it in exclude list? */
    if (xarray[30] == 0)
    {
        if ((distribution !=3) && (distribution !=5)) /* we are on linux */
        {
            retval = 0;
            retval = checkftp(out_file, distribution, verbose, html);
            if (retval < 0)
            {
                /* something went wrong...                      */
                perror("Problem in checkftp module.\n");
                perror("You should have seen errors...\n");
                /* module bombed, clean up */
                lsatrm();
                return(0);
            }
        }
    }

    /* did user put it in exclude list? */
    if (xarray[31] == 0)
    {
        retval = 0;
        retval = checklistening(out_file, distribution, verbose, html);
        if (retval < 0)
        {
            /* something went wrong..       */
            perror("Problem in checklistening module.\n");
            perror("You should have seen errors...\n");
            lsatrm();
            return(0);
        }
    }

    /* did user put it in exclude list? */
    if (xarray[32] == 0)
    {
        retval = 0;
	retval = checkdisk(out_file, verbose, html);
	if (retval < 0)
	{
	    /* something went wrong...      */
	    perror("Problem in checkdisk module.\n");
	    perror("You should have seen errors...\n");
	    lsatrm();
	    return(0);
	}
    }	

	/* did user put it in the exclude list? */
	if (xarray[33] == 0)
	{ 
		retval = 0;
		retval = checkservices(out_file, distribution, verbose, html);
		if (retval < 0)
		{
			/* something went wrong...		*/
			perror("Problem in checkservices module.\n");
			perror("You should have seen errors...\n");
			lsatrm();
			return(0);
		}
	}

    /* did user put it in the exclude list? */
    if (xarray[34] == 0)
    {
	retval = 0;
	retval = checkhostsfiles(out_file, verbose, html);
	if (retval < 0)
	{
	    /* something went wrong...			*/
	    perror("Problem in checkhostsfiles module.\n");
	    perror("You should have seen errors...\n");
	    lsatrm();
	    return(0);
	}
    }

    /* did user put it in exclude list? */
    if (xarray[35] == 0)
    {
        if ((distribution !=3) && (distribution != 5)) /* we are on linux */
        {
            retval = 0;
            retval = checkpkgupdate(out_file, distribution, verbose, html);
            if (retval < 0)
            {
                /* something went wrong...                      */
                perror("Problem in checkpkgupdate module.\n");
                perror("You should have seen errors...\n");
                /* module bombed, clean up */
                lsatrm();
                return(0);
            }
        }
    }

	/* did user put it in exclude list? */
    if (xarray[36] == 0)
    {
        if ((distribution !=3) && (distribution != 5))
        {
            /* we are on linux... */
            retval = 0;
            retval = checkpasstime(out_file, distribution, verbose, html);
            if (retval < 0)
            {
                perror("Problem in checkpasstime module.\n");
                perror("You should have seen errors...\n");
                lsatrm();
                return(0);
            }
        }
    }


    /* this is to put a footer html at the end of the output */
    /* file if the user wanted html output. 		     */
    if (html == 1)
    {
        html = 3; /* tells dostuff to do an html footer */
        header = "\n"; /* can't be NULL */
        if ((dostuff(0, out_file, 0, header, html)) < 0)
        {
            /* something went wrong */
            perror(" Creation of list failed.");
            return (-1);
        }
        html = 1; /* set html int back... habit */
    }
    else
    {
        /* print a general footer */
	header = "\n";
	if ((dostuff(0, out_file, 0, header, 0)) < 0)
	{
	    /* something went wrong */
	    perror(" Creation of footer failed.");
	    return(-1);
	}
    }
    

    /* tell the user we are finished and where the output is */
    /* we print this out no matter what verbose is */
    printf("Finished.\n");
    printf("Check %s for details.\n", out_file);
    /* in silent mode we don't say much */
    if (verbose >= 0)
    {
        printf("Don't forget to check your umask or file perms\n");
        printf("when modifying files on the system.\n");
    }

    /* set the umask back... */
    umask(old_umask);
    return(0);
}


/**********************************************************************
Copyright (C) 2002-2014 Robert Minvielle, Ph.D. (number9)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
**********************************************************************/
