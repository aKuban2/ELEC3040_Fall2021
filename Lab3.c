#include "stm32l4xx.h"

/*Global variables*/

static int counter1 = 0; 	        //counts up or down
static int counter2 = 0; 	        //only counts up

static unsigned char PB3 = 0;
static unsigned char PB4 = 0;
static unsigned char up = 0; 	    // =1 countAincrements; =0 countA decrements
static unsigned char run = 0;   	// = 1 counters begin

/*Function Definitions*/



/*Delay Function*/


void delay(){
   int i,j,n;
    for (i=0;i<20;i++){
        for(j=0;j<20000;j++) {
	        n=j;
														}
											}
						}



/* Configure port PA1 as falling edge interrupton EXTI1 */

void PA1Setup()
{
/* Enable SYSCFG clock */
    RCC->APB2ENR |= 0x01; 				//Enables GPIOA clock (bit 0)

/* Select PA1 as EXTI1 */

    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI1; 	// clear EXTI1 bit in config reg
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PA; 	// set PA1 as interrupt source

/* configure and enable EXTI1 as falling-edge triggered */

    EXTI->FTSR1 |= EXTI_FTSR1_FT1; 			// EXTI1 = falling-edge triggered
    EXTI->PR1 = EXTI_PR1_PIF1; 			    // clear EXTI1 pending bit
    EXTI->IMR1 |= EXTI_IMR1_IM1; 			// Enable EXTI1

/* Program NVIC to clear pending bit and enale EXTI1 */

    NVIC_ClearPendingIRQ(EXTI1_IRQn); 		// Clear NVIC pending bits
    NVIC_EnableIRQ(EXTI1_IRQn); 			// Enable IRQ
}



/*Configure port PA2 as falling edge interruption EXTI2 */

void PA2Setup() 
{
/* Enable SYSCFG clock */

    RCC->APB2ENR |= 0x01; //Enables GPIOA clock (bit 0)
/* Select PA2 as EXTI2 */
    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI2;     // clear EXTI2 bit in config reg
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PA;   // set PA2 as interrupt source

/* configure and enable EXTI2 as falling-edge triggered */

    EXTI->FTSR1 |= EXTI_FTSR1_FT2;                  // EXTI2 = falling-edge triggered
    EXTI->PR1 = EXTI_PR1_PIF2;                      // clear EXTI2 pending bit
    EXTI->IMR1 |= EXTI_IMR1_IM2;                    // Enable EXTI2

/* Program NVIC to clear pending bit and enable EXTI1 */

    NVIC_ClearPendingIRQ(EXTI2_IRQn);               // Clear NVIC pending bits
    NVIC_EnableIRQ(EXTI2_IRQn);                     // Enable IRQ
}

/* Pin Setup */
void PinSetup()
{
    RCC->AHB2ENR |= 0x01;               // Enables GPIOA clock (bit 0)

	
	//10987654321098765432109876543210
	//5544 3322 1100 9988 7766 5544 3322 1100
	//xxxx xx01 0101 0101 0101 01xx xx00 00xx
	//0    1    5    5    5    4    0    0
	//0    2    A    A    A    8    3    C
	
    GPIOA->MODER &= ~0x02AAA83C;       // input mode               ~(0011 1111 1111 1111 1111 0011 1100)
    GPIOA->MODER |= 0x01555400;      // set output                 0001 0101 0101 0101 0101 0000 0000
    RCC->AHB2ENR |= 0x02;               // Enables GPIOB clock (bit1)
    GPIOB->MODER &=~0x0000FFFF;       // Resets pins for output   ~(0000 0000 0000 1111 1111 1111 1111)
    GPIOB->MODER |=0x0005555;         // Set Output                 0000 0000 0000 0101 0101 0101 0101
}


/*------------------------------------------------*/
/* Implements count function */
/*------------------------------------------------*/


void counting()
{
   if (run)
   {
        if(counter2==9)	{
						counter2=0;
												}

				else{
					counter2++;
						}

        if(up)
        {
            if (counter1 != 9)  
            {
                counter1 ++;
            }
            else 
            {
                counter1 = 0;          //statement to cause decading
            }
        }
        else
        {
            if (counter1 != 0) 
            {
                counter1 --;
            }
            else 
            {
                counter1 = 9;          //statement to cause decading
            }
        }
   }        

}

/* Interrupt Functions */


void EXTI1_IRQHandler()
{
    __disable_irq();

/* Toggle PB3 */

    if (PB3==1)
    {   
        PB3=0;  
    }
    else    
    {
        PB3=1;  
    }

/* Toggle up */

    if (up==1)
    {
        up=0;
    }
    else
    {
        up=1;
    }
    EXTI->PR1 = EXTI_PR1_PIF1;          // clear EXTI1 pending bit
    NVIC_ClearPendingIRQ(EXTI1_IRQn);   // Clear NVIC pending bits
    __enable_irq();
}


void EXTI2_IRQHandler()
{
    __disable_irq();

/* Toggle PB4 */

    if (PB4==1)
    {
        PB4=0;  
    }
    else
    {
        PB4=1;
    }

/* Toggle run */

    if (run==1)
    {
        run=0;
    }
    else
    {
        run=1;
    }
    EXTI->PR1 = EXTI_PR1_PIF2;          // clear EXTI1 pending bit
    NVIC_ClearPendingIRQ(EXTI2_IRQn);   // Clear NVIC pending bits
    __enable_irq();
}
/* Main Program */
int main(void)
{
    PinSetup();
    PA1Setup();
    PA2Setup();
    __enable_irq();

    while(1)    
    {                                                                   // Endless Loop
        delay();
        counting();
        GPIOA->ODR = ((counter1 & 0x0F) << 5) | ((counter2 & 0x0F)<<9 );
        GPIOB->ODR = (((PB3<<3) | (PB4)<<4) | (GPIOA->ODR &~(0x30)));   // Setting PBs as outputs
    }
    return 0;
}
