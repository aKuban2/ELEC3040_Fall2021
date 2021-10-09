
#include "stm32l4xx.h" /* Microcontroller information */
/* Define global variables */
static unsigned int count_1 = 0; /* value of counter */
static unsigned int count_2 = 0;
static unsigned int sw1 = 0;
static unsigned int sw2 = 0;
/*---------------------------------------------------*/
/* Initialize GPIO pins used in the program */
/* PA1 = sw1, PA2 = sw2 */
/* PB[4:2] = output */
/*---------------------------------------------------*/
void PinSetup () {
/* Set PA1 and PA2 to be inputs */
   RCC->AHB2ENR |= 0x01; /* Enable GPIOA clock (bit 0) */
   GPIOA->MODER &= ~(0x03FFFC3C); /* General purpose input mode */
GPIOA->MODER |= (0x1555400); //GPIOA pin clear
}
/*------------------------------------------------*/
/* Implements count function */
/*------------------------------------------------*/
void count() {
   if (sw2 != 0) {
      if (count_1 != 0) {
         count_1 = count_1 - 1;
      }
      else {
         count_1 = 9;
      }
   }
   else {
      if (count_1 == 9) {
         count_1 = 0;
      }
      else {
         count_1= count_1 + 1;
      }
   }
if(count_2 == 9)
{
count_2 = 0;
}
else
{
count_2 = count_2 + 1;
}
/* Reset bits and Set bits */
   GPIOA->ODR &= ~(0x1FE0);  
   GPIOA->ODR |= ((count_value2 <<4) | count_value) <<5;
}
/*----------------------------------------------------------*/
/* Implement Delay function  */
/* Do nothing for half a second. */
/*----------------------------------------------------------*/
void delay () {
   int i,j,n;
   for (i=0; i<50; i++) { //Half second delay implemented
      for (j=0; j<20000; j++) {
         n=j;
      } //do nothing
   }
}
/*------------------------------------------------*/
/* Main program */
/*------------------------------------------------*/
int main(void) {
   count_value = 0;
count_value2 = 0;
   sw1 = 0;
   sw2 = 0;
   PinSetup();
   while (1) {
 delay(); // Calls Delay function in main.
      sw1 = GPIOA->IDR & 0x00000002; //Endless loop 
      sw2 = GPIOA->IDR & 0x00000004;
      if (sw1 != 0) {
         count();
      }
   } /* repeat forever */
}
