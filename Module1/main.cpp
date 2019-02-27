/************************************************************************//**
* \file main.c
* \brief LAB EXERCISE 5.2 - SQUARE ROOT APPROXIMATION
*
* Write an assembly code subroutine to approximate the square root of an 
* argument using the bisection method. All math is done with integers, so the 
* resulting square root will also be an integer
******************************************************************************
* GOOD LUCK!
****************************************************************************/

#include "stdint.h"
 
 /** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#include "string.h"
/** @endcond */
 /**
* @brief 
*	The function will take a given number (x) and using the bisection method it will find the low (low^2 < x) and the high (high^2 > x).
* The function will then add the low and the high then divide the added number by two.  The result of that number will then be put
* back into the function as the high to be looped through until the resulting answer is 1.  When that answer is 1, the previous high
* is the square root of x and will be returned in register 0.
*
* @note
*
* @param[in] 
*  x: input number
*
* @return
*  r0: square root of x (truncated if sqrt(x) is a floating point)
* 
*/
__asm int my_sqrt(int x){
	MOVS   r1, r0         ; Set high bound to x
	MOVS   r6, r0         ; move x into r6
    MOVS   r0, #0         ; Set the low bound
                
loop
	ADDS   r2, r1, r0 	  ; low + high
    LSRS   r3, r2, #1     ; mid = (low + high)/2
    MOVS   r5, r3         ;
    MULS   r5, r5, r5     ; square the mid
    CMP    r5, r6         ;	compare (mid^2) and int x
    BLT    move_low       ;
    BGT    move_high	  ;

FINISH
    MOVS   r0, r3         ; If the mid is equal to x, push r4 and return to main()
    BX     lr                                                                                                       ;

move_high
	CMP	   r1, r3		  ; Check that the high value is not already the mid value
	BEQ	   FINISH
	MOVS   r1, r3         ; set the high bound to the mid
    B      loop           ;

move_low
	CMP	   r0, r3		  ; Check that the low value is not already the mid value
	BEQ	   FINISH
	MOVS   r0, r3         ; set the low bound to the mid
    B      loop			  ;
}

/*----------------------------------------------------------------------------
MAIN function
*----------------------------------------------------------------------------*/
/**
* @brief Main function
*
* Detailed description of the main
*/
int main(void){
	volatile int r, j=0;
	int i;   
	r = my_sqrt(2);
	r = my_sqrt(4);     
	r = my_sqrt(22);
	r = my_sqrt(121);
  for (i=0; i<10000; i++){
    r = my_sqrt(i);
    j+=r;
  }
  while(1);
}

// *******************************ARM University Program Copyright � ARM Ltd 2014*************************************/
