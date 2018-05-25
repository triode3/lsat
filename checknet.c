/************************************************/
/* This checks open network ports.		*/
/* This module checks for open network ports on */
/* the system by means of a netstat -an.	*/
/* the output is stripped for LISTEN lines and  */
/* that is checked against /etc/services in     */
/* order to print out a "guess" at what is 	*/
/* running on that port. I could have used nmap */
/* or something else, but I wanted to just have */
/* a quick check without having to install 	*/
/* yet another program (YAP).			*/
/*						*/
/* See lsatmain.c for more details...		*/
/************************************************/
#include "lsatheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int checknet(filename, distribution, verbose, html)
const char *const filename;
int distribution;
int verbose;
int html;
{

    char *ptr;            /* char ptr for strtok		*/
    char line[192];      /* temp line for reading		*/
    char tempstring[192];  /* tempstring for entries	*/
    int field;  			 /* some vars...	*/
    int i;					 /* counter	*/
    int ipfield;        /* where is the location of the port in the output of netstat/ss? */
    FILE *infile;  		 /* tempfile file pointer   	*/
    FILE *fileptr;
    const char * tmp_file = "/tmp/lsat2.lsat";

    const char * tempfile =NULL;
    const char * shellcode=NULL;
    const char * header   =NULL;

    if (verbose >= 0)
    {
        printf(" Running checknet module...\n");
    }

    if (verbose > 0)
    {
        printf(" Checking for open/running network services...\n");
    }


    if (distribution != 3)
    /*  not on Solaris, go for it */
    {
    
        tempfile = "/tmp/lsat1.lsat";
        if ((system("test -r /bin/netstat")) == 0)
        {
            /* Since netstat seems to be on the system, lets use that. */
            ipfield = 5;
            shellcode = "netstat -an |grep LISTEN |grep 0.0.0.0 2>/dev/null >>/tmp/lsat1.lsat";
        }
        else /* We are assuming that netstat is gone and we are on iproute2 */
        {
            /* this may be a bad assumption, but if you do not have netstat */
            /* and you do not have ss, I don't know what to do with you :)  */
            /* in that case if will dostuff will fail with the perror       */
            ipfield = 4;
            shellcode = "ss -anp |grep -v \"* 0\" |grep -v WAIT 2>/dev/null >>/tmp/lsat1.lsat";
        }
        if ((dostuff(tempfile, 0, shellcode, 0, html)) < 0)
        {
 	    /* something went wrong */
	    perror(" Creation of list failed.");
   	    return(-1);
        }


    /* create another temp file for storage */
    if ((fileptr = fopen(tmp_file, "a")) == NULL)
    {
        perror("Could not make file w/perms 0600...");
        perror("Possible link attack while creating/opening file!");
        perror("Bailing in checknet...");
        exit(-1);
    }

    /* ok, open the file dostuff just made for read */
    infile = fopen("/tmp/lsat1.lsat", "r");
    if (infile == NULL)
    {
        perror("Error opening tmp file.");
        perror("Error was in module: checknet.");
        return(-1);
    }

    
    /* ok, for netstat the file should look something like: 	*/
    /* protocol	n	n	<ip>:<port>      <ip>:<port> LISTEN 	*/
    /* for ss the port is in field _4_ not _5_, called ipfield below    */
    /* so we want the port on the 1st <ip>:<port> pair.		    */
    /* we then want to look that port up in /etc/services    */
    /* and print out that line to the outfile...			*/

    i = 0; /* counter for our array */
    while (fgets(line, sizeof(line), infile) != NULL)
    {
        field=0;
        /* if we say a token is a ' ' or :, then we need to 	*/
        /* grab the 5th field of each line of the file.		*/
        /* note here we will also print out the 1st field,	*/
        /* although AFAIK they will all be tcp.			*/
        ptr = strtok(line, ", :\n");
        /* do not read in past field 5, buffer overflow can	*/
        /* exist, thanks nordi.					*/
        while ((ptr != NULL) && (field < 5))
        {
            if (sscanf(ptr, "%191s", tempstring) < 1)
            {
                perror("sscanf bit the dust");
                perror("Dust biting was in checknet.\n");
                return(-1);
            }
            /* get the 1st field...*/
            if (field == 0)
            {
		fprintf(fileptr, "%s\t\t", tempstring);
            }
            /* get the 5th field... */
            /* which is actually 5-1=4, but we are using */
            /* the variable ipfield...                      */
            if (field == (ipfield-1))
            {
		fprintf(fileptr, "%s\n", tempstring);
            }
            ptr = strtok(NULL, ", :\n");
            if (ptr == NULL)
            {
                /* freak out */
                break;
            }
            field++;
        } /* end while (ptr!=NULL) */
    } /* end while(fgets...) */

    /* close infile  */
    fclose(infile);
    /* rm infile as dostuff is going to read in tmpfile */
    remove(tempfile);
    /* dont forget to close tmpfile or dostuff will crash */
    fclose(fileptr);

    header = "Check these ports in /etc/services to see what they are.\nClose all ports you do not need.\n\nPorts listening on this system:\nProtocol\tPort\n";
    if ((dostuff(tmp_file, filename, 0, header, html)) < 0)
    {
	/* something went wrong */
	perror(" Creation of list failed.");
	return(-1);
    }
    
    /* this ends the if (distribution != 3) function */
    }

    if (distribution == 3)
    /* we are on Solaris, begin getting funky */
    {
	tempfile = "/tmp/lsat1.lsat";
        shellcode = "netstat -a -f inet |grep LISTEN |awk -F\" \" 'length($1) > 0 {print $1}' 2>/dev/null >> /tmp/lsat1.lsat";
        header = "These ports were found to be listening on the system.\nClose all ports or services you do not need.";
        if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
	{
	    /* something went wrong */
	    perror(" Creation of list failed.");
	    return(-1);
	}
    }

    /* we shall attempt to do a check with nmap just for fun */
    /* nmap has nice output, and is a very good program...   */

    if ((system("which nmap 1>/dev/null 2>/dev/null >/dev/null") == 0))
    {
	/* it should be installed and in the PATH */
	/* TODO */
	/* we need to check if there is more than */
	/* one interface on the box and then nmap */
	/* each interface, except for loopback    */

        /* also note, some distrios outputs are different */
        if (distribution == 5)
	{
	    	/* we are on Mac OSX */
	    	shellcode = "nmap -v -T insane `ifconfig | grep inet | grep -v inet6 | awk -F\" \" 'length($2) > 0 {print $2}' |grep -v 127.0.0.1 |xargs` 2>/dev/null  >>/tmp/lsat1.lsat";
        }
		if (distribution == 4) 
		{
	   	    /* we are on gentoo */
		    /* for some reason, I note that "old" gentoo puts ifconfig is /sbin/ and "new" gentoo puts ifconfig in /bin */
		    if ((system("test -r /bin/ifconfig")) == 0)
		    {
	    		shellcode = "nmap -v -T insane `/bin/ifconfig |grep inet |awk -F\" \" 'length($2) > 0 {print $2}' |awk -F \":\" 'length($2) > 0 {print $2}' |grep -v 127.0.0.1 |xargs` 2>/dev/null  >>/tmp/lsat1.lsat";
		    }
		    if ((system("test -r /sbin/ifconfig")) == 0)
		    {
			shellcode = "nmap -v -T insane '/sbin/ifconfig |grep inet |awk -F\" \" 'length($2) > 0 {print $2}' |awk -F\":\" 'length($2) > 0 {print $2}' |grep -v 127.0.0.1 |xargs` 2>/dev/null >>/tmp/lsat1.lsat";
		    }    
                }   
        if (distribution == 3)
		{
	    	/* we are on solaris */
	    	/* hrm, is this going to work? */
	    	shellcode = "nmap -v -T insane `grep loghost /etc/hosts |awk -F\" \" 'length($1) > 0 {print $1}' |xargs` 2>/dev/null >>/tmp/lsat1.lsat";
        }
		if ((distribution == 1) || (distribution == 2) || (distribution == 7) || (distribution == 8))
		{
	    	/* we assume a redhat deriviative */
		/* tested in mint, should work in debian/ubuntu also */
 	    	shellcode = "nmap -v -T insane `/sbin/ifconfig |grep inet |grep -v 127.0.0.1 |awk -F\" \" 'length($2) > 0 {print $2}' |awk -F\":\" 'length($2) >0 {print $2}' |xargs` 2>/dev/null >>/tmp/lsat1.lsat";
        }
        header = "Output from nmap run on local IP(s)\nCheck these services to see if they are critical.\nDisable services you do not need.\n"; 
        if ((dostuff(tempfile, filename, shellcode, header, html)) < 0)
        {
	    	/* something went wrong */
            perror(" Creation of list failed.");
	    	return(-1);
        }

	/* run arp to see if we have some arp poisen going on */
	if ((system("which arp 1>/dev/null 2>/dev/null > /dev/null")) == 0)
	{
	    shellcode = "arp -an 2>/dev/null >>/tmp/lsat1.lsat";
	    header = "Output from arp -a. \nIf you have arp poisoning, it should show up here.\n";
	    if ((dostuff(tempfile, filename, shellcode, header, html)) <0)
	    {
	        /* something went wrong */
	        perror(" Creation of list failed.");
	        return(-1);
	    }
	}
	/* check for kernel interface statistics */
	if ((system("which netstat 1>/dev/null 2>/dev/null > /dev/null")) == 0)
	{
	    shellcode = "netstat -i 2>/dev/null >/tmp/lsat1.lsat";
	    header = "Output from netstat -i showing Kernel interface statistics\n";
	    if ((dostuff(tempfile, filename, shellcode, header, html)) <0)
	    {
	        /* something went wrong */
	        perror(" Creation of list failed.");
		return(-1);
	    }
	}

	/*check for routing*/
	if ((system("which netstat 1>/dev/null 2>/dev/null > /dev/null")) == 0)
	{
	    shellcode = "netstat -rn 2>/dev/null >/tmp/lsat1.lsat";
	    header = "Output from netstat -rn showing current routing\n";
	    if ((dostuff(tempfile, filename, shellcode, header, html)) <0)
	    {
	        /* something went wrong */
		perror(" Creation of list failed.");
		return(-1);
	    }
	}

    }
    if (verbose > 0)
    {
        printf(" Finished module checknet.\n");
    }
    return(0);
}
