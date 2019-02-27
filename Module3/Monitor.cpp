/**----------------------------------------------------------------------------
             \file Monitor.cpp
--                                                                           --
--              ECEN 5003 Mastering Embedded System Architecture             --
--                  Project 1 Module 3                                       --
--                Microcontroller Firmware                                   --
--                      Monitor.cpp                                            --
--                                                                           --
-------------------------------------------------------------------------------
--
--  Designed for:  University of Colorado at Boulder
--               
--                
--  Designed by:  Tim Scherr
--  Revised by:  Student's name 
-- 
-- Version: 2.0
-- Date of current revision:  2016-09-29   
-- Target Microcontroller: Freescale MKL25ZVMT4 
-- Tools used:  ARM mbed compiler
--              ARM mbed SDK
--              Freescale FRDM-KL25Z Freedom Board
--               
-- 
   Functional Description: See below 
--
--      Copyright (c) 2015 Tim Scherr All rights reserved.
--
*/              

#include <stdio.h>
#include "shared.h"
#define FLASH_BASE       *((volatile unsigned long *) (0x00000000UL)

extern uint32_t read_register(int x);

/*******************************************************************************
* Set Display Mode Function
* Function determines the correct display mode.  The 3 display modes operate as 
*   follows:
*
*  NORMAL MODE       Outputs only mode and state information changes   
*                     and calculated outputs
*
*  STOP MODE         No Outputs
*
*  DEBUG MODE        Outputs mode and state information, error counts,
*                    register displays, sensor states, and calculated output
*
*
* There is deliberate delay in switching between modes to allow the RS-232 cable 
* to be plugged into the header without causing problems. 
*******************************************************************************/

void set_display_mode(void)   
{
  UART_direct_msg_put("\r\nSelect Mode");
  UART_direct_msg_put("\r\n Type N for normal mode");
  UART_direct_msg_put("\r\n Type D for debug mode" );
	UART_direct_msg_put("\r\n Type X to stop the output");
	UART_direct_msg_put("\r\n Type R to display register contents R0-R15" );
	UART_direct_msg_put("\r\n Type M to display a portion of memory");
  UART_direct_msg_put("\r\n Type V to display code version\r\n");
  UART_direct_msg_put("\r\nSelect:  ");
}

//*****************************************************************************/
///  \fn void chk_UART_msg(void) 
///
//*****************************************************************************/
void chk_UART_msg(void)    
{
   UCHAR j;
   while( UART_input() )      // becomes true only when a byte has been received
   {                                    // skip if no characters pending
      j = UART_get();                 // get next character

      if( j == '\r' )          // on a enter (return) key press
      {                // complete message (all messages end in carriage return)
         UART_msg_put("->");
         UART_msg_process();
      }
      else 
      {
         if ((j != 0x02) )         // if not ^B
         {                             // if not command, then   
            UART_put(j);              // echo the character   
         }
         else
         {
           ;
         }
         if( j == '\b' ) 
         {                             // backspace editor
            if( msg_buf_idx != 0) 
            {                       // if not 1st character then destructive 
               UART_msg_put(" \b");// backspace
               msg_buf_idx--;
            }
         }
         else if( msg_buf_idx >= MSG_BUF_SIZE )  
         {                                // check message length too large
            UART_msg_put("\r\nToo Long!");
            msg_buf_idx = 0;
         }
         else if ((display_mode == STOP) && (msg_buf[0] != 0x02) && 
                  (msg_buf[0] != 'D') && (msg_buf[0] != 'N') && 
                  (msg_buf[0] != 'V') && (msg_buf[0] != 'R') &&
                  (msg_buf[0] != 'M') && (msg_buf_idx != 0))
         {                          // if first character is bad in Stop mode
            msg_buf_idx = 0;        // then start over
         }
         else {                        // not complete message, store character
 
            msg_buf[msg_buf_idx] = j;
            msg_buf_idx++;
            if (msg_buf_idx > 0)
            {
               UART_msg_process();
            }
         }
      }
   }
}

//*****************************************************************************/
///  \fn void UART_msg_process(void) 
///UART Input Message Processing
//*****************************************************************************/
void UART_msg_process(void)
{
   UCHAR chr,err=0;
//   unsigned char  data;

   if( (chr = msg_buf[0]) <= 0x60 ) 
   {      // Upper Case
      switch( chr ) 
      {
         case 'D':
            display_mode = DEBUG;
            UART_msg_put("\r\n Mode=DEBUG\n");
            display_timer = 0;
            break;

         case 'N':
            display_mode = NORMAL;
            UART_msg_put("\r\n Mode=NORMAL\n");
            display_timer = 0;
            break;

         case 'X':
            display_mode = STOP;
            UART_msg_put("\r\n Mode=STOP\n");
				    UART_msg_put("\r\nSelect: ");
            display_timer = 0;
            break;

         case 'V':
            display_mode = VERSION;
            UART_msg_put("\r\n");
            UART_msg_put( CODE_VERSION ); 
						UART_msg_put("\r\nSelect: ");
            display_timer = 0;
            break;
        
				 case 'R':
               display_mode = REGISTER;
               UART_msg_put("\r\n Mode=REGISTER VIEW\n");
               display_timer = 0;
            break;
               
        case 'M':
               display_mode = MEMORY;
               UART_msg_put("\r\n Mode=MEMORY VIEW\n");
               display_timer = 0;
            break;
				 
         default:
            err = 1;
      }
   }

   else 
   {                                 // Lower Case
      switch( chr ) 
      {
        default:
         err = 1;
      }
   }

   if( err == 1 )
   {
      UART_msg_put("\n\rError!");
   }   
   else if( err == 2 )
   {
      UART_msg_put("\n\rNot in DEBUG Mode!");
   }   
   else
   {
    msg_buf_idx = 0;          // put index to start of buffer for next message
      ;
   }
   msg_buf_idx = 0;          // put index to start of buffer for next message
}

//*****************************************************************************
///   \fn   is_hex
/// Function takes 
///  @param a single ASCII character and returns 
///  @return 1 if hex digit, 0 otherwise.
///    
//*****************************************************************************
UCHAR is_hex(UCHAR c)
{
   if( (((c |= 0x20) >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f'))  )
      return 1;
   return 0;
}

/*******************************************************************************
*   \fn  DEBUG and DIAGNOSTIC Mode UART Operation
*******************************************************************************/
void monitor(void)
{

/**********************************/
/*     Spew outputs               */
/**********************************/

   switch(display_mode)
   {
      case(STOP):
         {
            UART_msg_put("\r\n Mode=STOP\n");
					  UART_msg_put("\r\nSelect: ");
            display_flag = 0;
         }  
         break;
      case(VERSION):
         {
             display_flag = 0;
         }  
         break;         
      case(NORMAL):
         {
            if (display_flag == 1)
            {
               UART_msg_put("\r\nNORMAL ");
               UART_msg_put(" Flow: ");
               // ECEN 5803 add code as indicated
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               UART_msg_put(" Temp: ");
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               UART_msg_put(" Freq: ");
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               display_flag = 0;
            }
         }  
         break;
      case(DEBUG):
         {
            if (display_flag == 1)
            {
               UART_msg_put("\r\nDEBUG ");
               UART_msg_put(" Flow: ");
               // ECEN 5803 add code as indicated               
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               UART_msg_put(" Temp: ");
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               UART_msg_put(" Freq: ");
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               
               display_flag = 0;
             }   
         }  
         break;
		case(REGISTER):
         {
					 if(display_flag == 1){
						UART_msg_put("\r\nREGISTERS R0 - R15: ");	
						char mem_str[20];
						char number[3];
						UART_direct_msg_put("\r\n");
						for(int i = 0; i < 16; i++){
							uint32_t mem = read_register(i);
							sprintf(mem_str, "%08x", mem);
							sprintf(number,"%d", i);
							UART_direct_msg_put("R");
							UART_direct_msg_put(number);
							UART_direct_msg_put(" = 0x");
							UART_direct_msg_put(mem_str);	
							UART_direct_msg_put("\r\n");	
						}	
						 display_flag = 0;
					 }
				 }
         break;

    case(MEMORY):
         {
             if (display_flag == 1)
						 {
               UART_msg_put("\r\nMEMORY BLOCK: ");
							 uint32_t * sp_addr = (uint32_t *)__current_sp();
							 char sp_text[15];
							 sprintf(sp_text,"%08x", sp_addr);
							 UART_direct_msg_put("\r\nStack pointer: 0x");
							 UART_direct_msg_put(sp_text);
							 UART_direct_msg_put("\r\n");
							 char sp_addr_str[30];
							 char memory_val_str[30];
							 uint32_t memory_val = 0x0;
							 for (int i=0; i<16; i++)
							 {
								 sp_addr = sp_addr-0x4;
								 sprintf(sp_addr_str,"%08x", sp_addr);
								 memory_val = (uint32_t)*(sp_addr);
								 sprintf(memory_val_str,"%08x", memory_val);
								 UART_direct_msg_put("Address 0x");
								 UART_direct_msg_put(sp_addr_str);
								 UART_direct_msg_put(" = Data ");
								 UART_direct_msg_put("0x");
							   UART_direct_msg_put(memory_val_str);
							   UART_direct_msg_put("\r\n");
							 }
							  display_flag = 0;
             }
         }  
         break;
				 
      default:
      {
         UART_msg_put("Mode Error");
      }  
   }
}  
