#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BSIZE_SP 1000

main(int argc, char *argv[])
{
char buf[BSIZE_SP];
FILE *fp;
int gspice_abort_flag;

if (argc != 2)
   {
     printf("Error in 'check_ngspice_logfile': need one and only one argument\n");
     printf("  -> in this case, the number of arguments is %d\n",argc);
     exit(1);
   }
if ((fp = fopen(argv[1],"r")) == NULL)
   {
     printf("Error in 'check_ngspice_logfile': can't open file:\n '%s'\n",argv[1]);
     exit(1);
   }

gspice_abort_flag = 0;
while (fgets(buf, BSIZE_SP-1, fp)) 
   {
   if (strncmp(buf,"run simulation(s) aborted",25) == 0)
       {
	 gspice_abort_flag = 1;
	 break;
       }
   }

if (gspice_abort_flag == 1)
   {
   rewind(fp);
   printf("\n******************** ERROR ENCOUNTERED WITH NGSPICE RUN **********************\n\n");
   printf("--> Error lines from 'simrun.log' file:\n");
   while (fgets(buf, BSIZE_SP-1, fp)) 
      {
       if (strncmp(buf,"Error",5) == 0)
          {
	   printf(buf);
          }
      }
   printf("\n************************* VIEW LOG FILE FOR MORE DETAILS **************************\n");
   printf("\n------------------------------ Error:  ngspice command failed! --------------------------------\n");
   }
fclose(fp);
}
