// Check verilator result
// written by Michael H Perrott, March 2010
// No warranty or support provided for this code

#include <stdlib.h>
#include <stdio.h>


main(int argc,char *argv[])
{
FILE *fp;

if (argc != 4)
  {
   printf("Error: need three arguments!\n");
   printf("  check_verilator_run_status verilator_makefile module_name lib\n");
   exit(1);
  }


if ((fp = fopen(argv[1],"r")) == NULL)
     {
      printf("\n***** Error: Verilator unsuccessful on module '%s' (lib '%s') *****\n",
              argv[2], argv[3]);
      printf("***** --> please fix Verilog code of this module to address errors listed above *****\n");
      exit(1);
     }
else
   {
      fclose(fp);
      printf("\n--- Note: CppSim is OK with Verilator result on module '%s' (lib '%s') ---\n",
              argv[2], argv[3]);
      exit(0);
   }

}
