/**File: FloatAdd.c
 * Name: Jay Ofemi
 * Date: 02/15/2017
 * Collaboration Declaration:
 *   Online resources.
 */
#include <stdio.h>

//This is a FloatAdd routine, 
//meant to emulate IEEE 32-bit floating point addition.

///Function Declarations
//FloatAdd Function
int FloatAdd(int op1, int op2);
//Function to break out the three components (Sign, Exponent, 
//and Significand) and categorize the value.
unsigned int categorize(int operand, unsigned int sign, 
                          unsigned int exponent, unsigned int significand);
//Function to handle zero inputs
int zeroInputs(int op1, int op2, unsigned int category1, 
                  unsigned int category2, unsigned int sign1, 
                   unsigned int sign2);
//Function to handle infinty inputs
int infintyInputs(int op1, int op2, unsigned int sign1, unsigned int sign2,
                    unsigned int category1, unsigned int category2);
//Function to handle NaN inputs
int NaNInputs(int op1, int op2, unsigned int category1, 
                unsigned int category2);
//Function to add Normalized inputs
int addNormalized(int op1, int op2, unsigned int sign1, unsigned int sign2, 
                    unsigned int exponent1, unsigned int exponent2, 
                      unsigned int significand1, unsigned int significand2,
                          unsigned int denorm);
//Function to check for underflow
int checkForUnderflow(unsigned int exponent);
//Function to check for overflow
int checkForOverflow(unsigned int exponent);
//Function to get rounding factor (guard + sticky)
int guardPlusSticky(unsigned int significand, unsigned int sticky);


//FloatAdd Function
int FloatAdd(int op1, int op2)
{
  //local variables definition
  int result;
  unsigned int category1, category2, denorm;
  unsigned int sign1, exponent1, significand1;
  unsigned int sign2, exponent2, significand2;
  //turn debugging on (1) or off (0)
  #define DO_DEBUGGING_PRINTS 0

  //for operand 1
  //sign bit
  sign1 = (op1 & 0x80000000) >> 31;
  //Exponent bits
  exponent1 = (op1 & 0x7F800000) >> 23;
  //Significand bits
  significand1 = ((op1 << 9) >> 9) & 0x007FFFFF;
  //categorize the operand
  category1 = categorize(op1, sign1, exponent1, significand1);

  //for operand 2
  //sign bit
  sign2 = (op2 & 0x80000000) >> 31;
  //Exponent bits
  exponent2 = (op2 & 0x7F800000) >> 23;
  //Significand bits
  significand2 = ((op2 << 9) >> 9) & 0x007FFFFF;
  //categorize the operand
  category2 = categorize(op2, sign2, exponent2, significand2);
  
  //Input Handling
  denorm = 0;
  //handle NaN inputs
  if(category1 == 2 || category2 == 2)
  {
    result = NaNInputs(op1, op2, category1, category2);
  }
  //handle infinty inputs
  else if(category1 == 1 || category2 == 1)
  {
    result = infintyInputs(op1, op2, sign1, sign2, category1, category2);
  } 
  //handle zero inputs
  else if(category1 == 0 || category2 == 0)
  {
    result = zeroInputs(op1, op2, category1, category2, sign1, sign2);
  }
  //handle De-Normalized inputs
  else if(category1 == 4 && category2 == 4)
  {
    denorm = 1;
    exponent1 = exponent1 + 1;
    exponent2 = exponent2 + 1;
    //add
    result = addNormalized(op1, op2, sign1, sign2, exponent1,
                            exponent2, significand1, significand2, denorm);
  }
  
  //handle Normalized inputs
  //if op1 is normalized and op2 is not
  if(category1 == 3 && category2 == 4)
  {
    exponent2 = exponent2 + 1;
    //add 1 to significand 1
    significand1 = significand1 | 0x00800000;
    //add
    result = addNormalized(op1, op2, sign1, sign2, exponent1,
                            exponent2, significand1, significand2, denorm);
  }
  //if op2 is normalized and op1 is not
  else if(category1 == 4 && category2 == 3)
  {
    exponent1 = exponent1 + 1;
    //add 1 to significand 2
    significand2 = significand2 | 0x00800000;
    //add
    result = addNormalized(op1, op2, sign1, sign2, exponent1,
                            exponent2, significand1, significand2, denorm);
  }
  //if op1 is normalized and op2 is as well
  else if(category1 == 3 && category2 == 3)
  {
    //add 1 to significand 1
    significand1 = significand1 | 0x00800000;
    //add 1 to significand 2
    significand2 = significand2 | 0x00800000;
    //add
    result = addNormalized(op1, op2, sign1, sign2, exponent1,
                            exponent2, significand1, significand2, denorm);
  }

  //display necessary information to user
  //for operand 1
  #if DO_DEBUGGING_PRINTS
    fprintf(stderr, "-----Operand 1-----\n");
    fprintf(stderr, "Sign: %01x\n", sign1);
    fprintf(stderr, "Exponent: %08x\n", exponent1);
    fprintf(stderr, "Significand: %08x\n", significand1);
  #endif

  //for operand 2
  #if DO_DEBUGGING_PRINTS
    fprintf(stderr, "-----Operand 2-----\n");
    fprintf(stderr, "Sign: %01x\n", sign2);
    fprintf(stderr, "Exponent: %08x\n", exponent2);
    fprintf(stderr, "Significand: %08x\n", significand2);
  #endif
 
  return result;
}

/*Function to break out the three components (Sign, Exponent, 
 *and Significand) and categorize the value.
 * param: operand
 * param: sign
 * param: exponent
 * param: significand
 * return: 0 for zero value, 1 for Infinity, 2 for NaN, 
 *          3 for Normalized, and 4 for De-Normalized
 */
unsigned int categorize(int operand, unsigned int sign, 
                          unsigned int exponent, unsigned int significand)
{
  unsigned int value = 5;
  //check if exponent is all 0 bits
  if(exponent == 0x00)
  {
    //check if significand is also all 0 bits
    if(significand == 0x000000)
    {
      //then value is +/- zero
      value = 0;
      if(sign == 0)
      {
        #if DO_DEBUGGING_PRINTS
          fprintf(stderr, "Value %08x = +0\n", operand);
        #endif
      }
      else if(sign == 1)
      {
        #if DO_DEBUGGING_PRINTS
          fprintf(stderr, "Value %08x = -0\n", operand);
        #endif
      }
    }
    //if significand is not all 0 bits
    else
    {
      //value is denormalized
      value = 4;
      #if DO_DEBUGGING_PRINTS
        fprintf(stderr, "Value %08x is De-Normalized\n", operand);
      #endif
    }
  }
  //check if exponent is all 1 bits
  else if(exponent == 0xFF)
  {
    //check if significand is all 0 bits
    if(significand == 0x000000)
    {
      //then value is +/- infinity
      value = 1;
      if(sign == 0)
      {
        #if DO_DEBUGGING_PRINTS
          fprintf(stderr, "Value %08x = +Infinity\n", operand);
        #endif
      }
      else if(sign == 1)
      {
        #if DO_DEBUGGING_PRINTS
          fprintf(stderr, "Value %08x = -Infinity\n", operand);
        #endif
      }
    }
    //if significand is not all 0 bits
    else
    {
      //value is Not a Number(NaN)
      value = 2;
      #if DO_DEBUGGING_PRINTS
        fprintf(stderr, "Value %08x is NaN\n", operand);
      #endif
    }
  }
  //if exponent is neither all 0s nor all 1s
  else
  {
    //exponent is biased and value is Normalized
      value = 3;
      #if DO_DEBUGGING_PRINTS
        fprintf(stderr, "Value %08x is Normalized\n", operand);
      #endif
  }

  return value;
}

/*Function to handle zero inputs
 * param: operand 1
 * param: operand 2
 * param: category 1
 * param: category 2
 * param: sign 1
 * param: sign 2
 * return: zero value
 */
int zeroInputs(int op1, int op2, unsigned int category1, 
                unsigned int category2, unsigned int sign1, 
                  unsigned int sign2)
{
  //local variables
  int value;
  unsigned int sign;
  
  //if operand 1 is zero and operand 2 is not
  if(category1 == 0 && category2 != 0)
  {
    //value is operand 2
    value = op2;
  }
  //if operand 2 is zero and operand 1 is not
  else if(category2 == 0 && category1 != 0)
  {
   //value is operand 1
    value = op1;
  }
  //if operand 1 is zero and so is operand 2
  else if(category2 == 0 && category1 == 0)
  {
    //determine sign: positive takes priority over negative
    //if sign 1 is positive
    if(sign1 < sign2)
    {
      //use sign 1
      sign = sign1;
    }
    //if sign 2 is positive
    else if(sign1 > sign2)
    {
      //use sign 2
      sign = sign2;
    }
    //if sign 1 and 2 are the same
    else
    {
      //use sign 1
      sign = sign1;
    }
    //mask then or with the new sign bit
    value = ((op1 & 0x7FFFFFFF) | (sign << 31));
  }

  return value;
}


/*Function to handle infinty inputs
 * param: operand 1
 * param: operand 2
 * param: sign 1
 * param: sign 2
 * param: category 1
 * param: category 2
 * return: infinty value
 */
int infintyInputs(int op1, int op2, unsigned int sign1, unsigned int sign2,
                    unsigned int category1, unsigned int category2)
{
  //local variables
  int value;
  
  //if operand 1 is infinty and operand 2 is not
  if(category1 == 1 && category2 != 1)
  {
    //value is operand 1
    value = op1;
  }
  //if operand 2 is infinty and operand 1 is not
  else if(category2 == 1 && category1 != 1)
  {
   //value is operand 2
    value = op2;
  }
  //if operand 1 is infinity and so is operand 2
  else if(category2 == 1 && category1 == 1)
  {
    //and they both have the same signs
    if(sign1 == sign2)
    {
      //value is either operand
      value = op1;
    }
    //if their signs are different
    else
    {
      //value is Not a Number (NaN) : FFC00000
      value = 0xFFC00000;
    }
  }
  return value;
}

/*Function to handle Not a Number (NaN) inputs
 * param: operand 1
 * param: operand 2
 * param: category 1
 * param: category 2
 * return: NaN value
 */
int NaNInputs(int op1, int op2, unsigned int category1, 
                  unsigned int category2)
{
  //local variables
  int value;
  
  //if operand 1 is NaN and operand 2 is not
  if(category1 == 2 && category2 != 2)
  {
    //value is operand 1
    value = (op1 | 0x7FC00000);
  }
  //if operand 2 is NaN and operand 1 is not
  else if(category2 == 2 && category1 != 2)
  {
   //value is operand 2
    value = (op2 | 0x7FC00000);
  }
  //if operand 1 is NaN and so is operand 2
  else if(category2 == 2 && category1 == 2)
  {
    //value is Not a Number (NaN) : FFC00000
    value = (op2 | 0x7FC00000);
  }
  return value;
}

/*Function to add Normalized inputs
 * param: operand 1
 * param: operand 2
 * param: sign 1
 * param: sign 2
 * param: exponent 1
 * param: exponent 2
 * param: Significand 1
 * param: Significand 2
 * param: denorm
 * return: addition result value
 */
int addNormalized(int op1, int op2, unsigned int sign1, unsigned int sign2, 
                    unsigned int exponent1, unsigned int exponent2, 
                      unsigned int significand1, unsigned int significand2,
                          unsigned int denorm)
{
  //local variable Declarations
  unsigned int sign, exponent, significand, ogsignificand; 
  unsigned int sticky, delta, value, roundFactor, subtracted;
  //handle special cases
  unsigned int specCase = 0;

  //determine the bigger value and use its sign
  //if value 1 is greater
  if(exponent1 > exponent2)
    {
      //use sign of value 1
      sign = sign1;
      //difference between the exponents
      delta = exponent1 - exponent2;
      //take significand of the smaller value for shifting
      significand = ((significand2 << 2) & 0x03FFFFFC);
      //take significand of bigger value for storage
      ogsignificand = ((significand1 << 2) & 0x03FFFFFC);
      //take exponent of smaller value for storage
      exponent = exponent2;
    }
    //if value 2 is greater
    else if(exponent1 < exponent2)
    {
      //use sign of value 2
      sign = sign2;
      //difference between the exponents
      delta = exponent2 - exponent1;
      //take significand of the smaller value for shifting
      significand = ((significand1 << 2) & 0x03FFFFFC);
      //take significand of bigger value for storage
      ogsignificand = ((significand2 << 2) & 0x03FFFFFC);
      //take exponent of smaller value for storage
      exponent = exponent1;
    }
    //if the values are the same
    else
    {
      //difference of the exponents
      delta = 0;
      //no need to shift significand
      if(significand1 < significand2)
      {
        //use sign of bigger value
        sign = sign2;
        //take smaller significand for storage
        significand = ((significand1 << 2) & 0x03FFFFFC);
        //take bigger significand for storage
        ogsignificand = ((significand2 << 2) & 0x03FFFFFC);
      }
      else if(significand1 > significand2)
      {
        //use sign of bigger value
        sign = sign1;
        //take smaller significand for storage
        significand = ((significand2 << 2) & 0x03FFFFFC);
        //take bigger significand for storage
        ogsignificand = ((significand1 << 2) & 0x03FFFFFC);
      }
      //if significands are the same
      else
      {
        //check to see if signs are the same
        if(sign1 == sign2)
        {
          //use either sign
          sign = sign1;
          //take either significand for storage
          significand = ((significand2 << 2) & 0x03FFFFFC);
          //take other significand for storage
          ogsignificand = ((significand1 << 2) & 0x03FFFFFC);
        }
        //if signs are different 
        else
        {
          //sign is positive since value is zero
          sign = 0;
          //take either significand for storage
          significand = 0x00000000;
          //take other significand for storage
          ogsignificand = 0x00000000;
          specCase = 1;
        }
      }
      //take exponent of either value for storage
      exponent = exponent1;
    }

    //if delta is negative
    if(delta < 0)
    {
      //make delta positive
      delta = -delta;
    }

    sticky = 0;
    roundFactor = 0;
    //shift bits to match
    int i = 0;
    while(i < delta)
    {
      //sticky bit
      if((significand & 0x00000001) == 0x00000001)
      {
        sticky = 1;
      }
      //shift significand to right by one bit
      significand = significand >> 1;
      //increase exponent to match
      exponent++;
      //get round factor
      roundFactor = guardPlusSticky(significand, sticky);
      i++;
    }
    
    //add the significands if signs are the same
    if(sign1 == sign2)
    {
      significand = ogsignificand + significand;
      subtracted = 0;      
    }
    //subtract the significands if signs are different
    else
    {
      significand = ogsignificand - significand;
      subtracted = 1;
    }
    //get round factor
    roundFactor = guardPlusSticky(significand, sticky);

    //check if significand is all zeros
    if(significand == 0x00000000)
    {
      //if so, return value is 0
      value = 0x00000000;
    }
    else
    {      
      ///check to see if significand result is normalized
      //if value is De-Normalized
      //and significand must be shifted left
      if((significand & 0xFFFFFFFF) < 0x02000000) 
      {
        while((significand & 0xFFFFFFFF) < 0x02000000)
        {
          if(denorm == 1 && exponent <= 0x01)
          {
            exponent = 0x00;
            specCase = 1;
            break;
          }
          //shift significand left by one bit
          significand = significand << 1;
          exponent--;
          //get round factor
          roundFactor = guardPlusSticky(significand, sticky);
          //check for Underflow
          if (checkForUnderflow(exponent) == 1)
          {
            //if underflow occurs, value is zero
            exponent = 0x00;
            //get rid of guard bits
            significand = 0x00000000;
            specCase = 1;
            break;
          }
        }
      }
      //if significand must be shifted right
      if((significand & 0xFFFFFFFF) > 0x03FFFFFF) 
      {
        while((significand & 0xFFFFFFFF) > 0x03FFFFFF)
        {
          //sticky bit
          if((significand & 0x00000001) == 0x00000001)
          {
            sticky = 1;
          }
          //shift significand right by one bit
          significand = ((significand >> 1) & 0x7FFFFFFF);
          exponent++;
          //get round factor
          roundFactor = guardPlusSticky(significand, sticky);
          //check for Overflow
          if (checkForOverflow(exponent) == 1)
          {
            //if overflow occurs, value is infinty
            exponent = 0xFF;
            //get rid of sig bits
            significand = 0x00000000;
            specCase = 1;
            break;
          }
        }
      }
      
      //get rid of guard bits
      significand = ((significand >> 2) & 0x00FFFFFF);
      //Handle rounding and re-normalization
      if(specCase != 1)
      { 
        //if guard bits are 10 and sticky bit is 0
        if(roundFactor == 2)
        {
          //round to even
          if((significand & 0x00000001) == 0x00000001)
          {
            significand = significand + 1;
          }
        }
        //if guard bits are 10 and sticky is 1 and we added
        else if((roundFactor == 3) && (subtracted == 0))
        {
          significand = significand + 1;
        }
        //if guard bits are 11, regardless of sticky
        else if(roundFactor == 4)
        {
          significand = significand + 1;
        }
      
        ///check to see if significand result is still normalized
        //if value is De-Normalized
        //if significand must be shifted right
        if((significand & 0xFFFFFFFF) > 0x00FFFFFF) 
        {
          while((significand & 0xFFFFFFFF) > 0x00FFFFFF)
          {
            //shift significand right by one bit
            significand = ((significand >> 1) & 0x7FFFFFFF);
            exponent++;
          
            //check for Overflow
            if (checkForOverflow(exponent) == 1)
            {
              //if overflow occurs, value is infinty
              exponent = 0xFF;
              //get rid of sig bits
              significand = 0x00000000;
              specCase = 1;
              break;
            }
          }
        }
      }

      significand = (significand & 0x007FFFFF);
      //put all three components together
      //if significand is normalized or after shifting
      value = (significand | ((exponent << 23) & 0x7F800000));
      //add sign
      value = ((value & 0x7FFFFFFF) | ((sign & 0x00000001) << 31));
    }
  
  return value;
}

/*Function to check for underflow
 * param: exponent
 * return: 0 if there is no underflow and 1 if there is underflow
 */
int checkForUnderflow(unsigned int exponent)
{
  unsigned int value = 0;
  if(exponent > 0x800000FE)
  {
    value = 1;
  }
  return value;
}

/*Function to check for overflow
 * param: exponent
 * return: 0 if there is no overflow and 1 if there is overflow
 */
int checkForOverflow(unsigned int exponent)
{
  unsigned int value = 0;
  if(exponent >= 0x000000FF)
  {
    value = 1;
  }
  return value;
}

/*Function to get rounding factor (guard + sticky)
 * param: significand
 * param: sticky
 * return: rounding factor (1 = truncate, 2 = RTE (except in one case)
 *         3 = add 1 (except in one case))
 */
int guardPlusSticky(unsigned int significand, unsigned int sticky)
{
  //local variables declaration
  unsigned int guard, value;
  //set guard bits
  guard = (significand & 0x00000003);
  //rounding factor
  value = guard + sticky;

  //special cases:
  //if guard is 01 and sticky is 1
  if((value == 2) && (guard == 1))
  {
    //set value to 1 so it is truncated
    //and not rounded to even
    value = 1;
  }
  //if guard is 11 and sticky is 0
  else if((value == 3) && (guard == 3))
  {
    //set value to 4 so it always adds 1
    value = 4;
  }

  return value;
}
