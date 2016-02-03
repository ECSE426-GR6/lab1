/**
   @file fputc_debug.c
   @brief Trying to redirect printf() to debug port
   @date 2012/06/25
*/
 
#include <stdio.h>
#include "stm32f4xx.h"
 
//frank was here
 
int fputc(int c, FILE *stream)
{
   return(ITM_SendChar(c));
}


