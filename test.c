#include <stdio.h>
#include <stdlib.h>

int FloatAdd(int, int);

int main(void)
{
  //--------------Comment out below this line to test multiple values-----------  
  // //To use hex numbers
  // int x = 0xDCFFFFF8;
  // int y = 0xD27FFFF8;
  // int iresult = FloatAdd(x, y);

  // //To use decimal numbers
  // // float x = -3.0; 
  // // float y = 1.0;
  // // int iresult = FloatAdd(*(int *) &x, *(int *) &y);

  // float fresult = (*(float *) &x) + (*(float *) &y);
  // printf("-----Test-----\n");
  // printf("%08x + %08x --> %08x (should be %08x)\n", *(int *) &x, *(int *) &y, iresult,
  //   *(int *)&fresult);

  //--------------Comment out above this line to test multiple values----------- 
  //--------------Comment out below this line to test just two values-----------  

  //Triple while loop To test all exponent values with random significands
  double wrong = 0;
  int loop = 10000;
  int i = 0;
  while(i < 256)
  {
    int j = 0;
    while(j < 256)
    {
      int n = 0;
      while(n < loop)
      {


        int rand1 = rand( );
        int rand2 = rand( );
        int x = ((rand1 & 0x800000) << 8) | ( i << 23) | (rand1 & 0x7fffff);
        int y = ((rand2 & 0x800000) << 8) | ( j << 23) | (rand1 & 0x7fffff);


        //When testing through all exponents this will only print out the ones you got wrong.
        //To print every value just get rid of the if statement

        int iresult = FloatAdd(x, y);
        float fresult = (*(float *) &x) + (*(float *) &y);


        if( iresult != *(int *)&fresult )// COMMENT OUT if statement to test just two values
        {
          printf("%08x + %08x --> %08x (Should be %08x)\n", x, y, iresult,
            *(int *)&fresult);
          wrong++;
        }
         n++;
      }
       j++;
    }
    i++;
  }
  double total = 256 * 256 * loop;
  double right = total - wrong;
  double percentage = (right * 100)/total ;
  fprintf( stderr, "Total: %f Combinations\n", total );
  fprintf( stderr, "Wrong: %f Answers\n", wrong );
  fprintf( stderr, "Accuracy: %f percent\n", percentage);
  if(percentage < 100)
  {
    fprintf( stderr, "DO BETTER!\n");
  }
  else
  {
    fprintf( stderr, "WELL DONE!\n");
  }

  //--------------Comment out above this line to test just two values-----------
  return 0;
}