/*
This is a function that does all the filehandling stuff for the lsat modules so that
you don't have to reinvent the wheel every time you want to do something. After all
we have to get rid off, eradicate and stamp out all kinds of redundancies.

As arguments it takes four pointers to strings: tempfile, outfile, shellcode and
header. Additionally, it takes one other argument, html, which is an int. Header can be NULL, then it simply will not be printed. One of the other
pointers can be NULL as well. In this case refer to the following list:

no NULL pointer            default behaviour
  create tempfile
  run shellcode
  append header to outfile
  append tempfile to outfile
  delete tempfile

outfile == NULL            if you want to keep working with tempfile
  create tempfile
  run shellcode

tempfile == NULL           if you already have a tempfile and want to keep it
  run shellcode

shellcode == NULL          if you already got a tempfile but now you're finished
  append header to outfile
  append tempfile to outfile
  delete tempfile
  
Special Case:		   just print the header to the outfile
tempfile==shellcode==NULL
  append header to outfile

html = 0		   Output is normal
html = 1		   Output is in html format
html = 2		   First time we ran dostuff, print html header
html = 3		   Final part, print html footer
  
  
Will return 0 on success and negative value on failure.
*/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "lsatheader.h"

int dostuff (tempfile, outfile, shellcode, header, html)
const char *const tempfile;
const char *const outfile;
const char *const shellcode;
const char *const header;
int html;
{
    int  fileval=0;	/* return val */
    char line[512];	/* for reading lines		*/
    FILE *out;		/* file pointer for outfile	*/
    FILE *tmp;		/* file pointer for tempfile	*/


/* create a temp file for storage */
if ((tempfile!=NULL)&&(shellcode!=NULL))
{
    if ((fileval = open(tempfile, O_RDWR | O_CREAT | O_EXCL, 0600)) < 0)
    {
        perror("  Could not create temporary file...\n");
        perror("  Possible link attack while creating/opening file!\n");
        exit(-1);
    }
}


/* run the shellcode */
if (shellcode!=NULL)
{
    if ((system(shellcode)) < 0 )

    {
        perror("  Problem running the shellcode or writing to tempfile\n");
	/* sanity check */
        printf("  This was shellcode: %s\n",shellcode);
        return(-1);
    }
}


/* output the header (if given) and append tempfile to outfile and remove tempfile*/
if (outfile!=NULL)  /* &&(tempfile!=NULL)) */
{
    /* open outfile for append */
    out = fopen(outfile, "a");
    if (out == NULL)
    {
        perror("  Error opening outfile!\n");
        return(-1);
    }
    
    /* print header to outfile if given */
    /* Check to see if html = 1; if it is, do html. */
    if (!(header==NULL))
    {
	/* if html = 2, this is the first time we print anything */
	/* so we want to print the html headers... */
	if (html == 2)
	{
	    fprintf(out, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n");
            fprintf(out, "\"http://www.w3.org/TR/html4/loose.dtd\">\n");
	    fprintf(out, "<html>\n");
	    fprintf(out, "<head>\n");
	    fprintf(out, "<meta http-equiv=\"Content-type\" content=\"text/html;charset=UTF-8\">\n");
	    fprintf(out, "<title>LSAT output page</title>\n");
	    fprintf(out, "</head>\n");
	    fprintf(out, "<body bgcolor=\"#000000\" text=\"#FFFFFF\">\n");
	    fprintf(out, "<center>\n");
	    fprintf(out, "<h2><b>LSAT output page</b></h2>\n");
	    fprintf(out, "<br><br></center>\n<pre>\n");
	}
	/* if html == 1 the user wants all parts in html  */
	if (html == 1)
	{
	    fprintf(out, "<br><hr>\n");
	    fprintf(out, "<b><i>\n");
	    fprintf(out, "<font color=\"#00FF00\">\n");
	    fprintf(out, "%s\n", header);
	    fprintf(out, "</font>\n");
	    fprintf(out, "</i></b>\n");
	}
        /* no html, please  */
	if (html == 0)
	{
	    fprintf(out, "\n****************************************\n");
            fprintf(out, "%s\n", header);
	}
        /* if html == 3 this is the last one, but we still */
	/* need to print the last header... */
	if (html == 3)
	{
	    fprintf(out, "<br><hr>\n");
            fprintf(out, "<b><i>\n");
	    fprintf(out, "<font color=\"#00FF00\">\n");
            fprintf(out, "%s\n", header);
	    fprintf(out, "</font>\n");
            fprintf(out, "</i></b>\n");
	    fprintf(out, "</pre>\n</body>\n</html>\n");
	}
    }

    /* do tempfile stuff if given */
    if (tempfile!=NULL)
    {
        /* open tempfile for reading */
        tmp = fopen(tempfile, "r");
        if (tmp == NULL)
        {
            perror("  Error opening tempfile for read!\n");
	    fclose(out);
            return(-1);
        }

        /* append everything in tempfile to outfile */
	/* note to self, mod this to use getline in the future. */
	/* note2, fgets can crash here if sizeof(line) > 512. */
	/* this is really bad, causes segfault... need fix. lazy. */
        while (fgets(line, sizeof(line), tmp) != NULL)
        {
            fprintf(out, "%s", line);
        }

        /* close the tempfile and rm it */
        fclose(tmp);
        remove(tempfile);
        }
    
    /* close up */
    fclose(out);
}


    return(0);
}
