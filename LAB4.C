#include "stm32l4xx.h" /* microcontroller information */

/* Define global variables */
static unsigned int counter; //value of count (0-9)
static unsigned int button; //value of button press
static unsigned int col; //what column has been pressed
static unsigned int row; //what row has been pressed
static unsigned int colNum; //what column # has been pressed
static unsigned int rowNum; //what row # has been pressed
static unsigned int go; //handler variable
unsigned int i,j,n,k; //delay variables
static unsigned int keypad_map [4][4] = {
{0x01,0x02,0x03,0x0A}, //0,0;1st row
{0x04,0x05,0x06,0x0B}, //1,0;2nd row
{0x07,0x08,0x09,0x0C}, //2,0;3rd row
{0x0E,0x00,0x0F,0x0D} //3,0;4th row
};

/*---------------------------------------------------*/
/* initialize clocks used in the program */
/* initialize GPIOB pins used in the program */
/* PB[0] = interrupt trigger, output of AND gate (row signals) */
/* PB[6:3] = displayed value, counter or button */
/*---------------------------------------------------*/

void Setup() {
RCC->AHB2ENR |= 0x03;


/* configure GPIOB pins */

//     xxxx xxxx xxxx xxxx
//     7766 5544 3322 1100
// &= ~xx11 1111 11xx xx11 --- 3FC3
// 	----------------------------------
//     xx00 0000 00xx xx00 
// |=  xxx1 x1x1 x1xx xxxx --- 1540
// ---------------------------------
//     XX01 0101 01XX XX00 -- PB[6:3] OUTPUT PB[0] INPUT


GPIOB->MODER &= (0xFFFFC03C); //inputs// display and AND, PB[6:3,0] = 00
GPIOB->MODER |= (0x00001540); //outputs// display, PB[6:3] = 01
}


void PinSetup1() {
     Setup();
/* configure GPIOA pins */

//    DDCC BBAA 9988 7766 5544 3322 1100
//	  xxxx xxxx xxxx xxxx xxxx xxxx xxxx
// &=~xxxx 1111 1111 xxxx 1111 1111 xxxx --- 0FF0FF0*******NEEEDED FOR INPUT SET
// ----------------------------------------
//    xxxx 0000 0000 xxxx 0000 0000 xxxx
// |= xxxx x1x1 x1x1 xxxx x1x1 x1x1 xxxx --- 0550550
//----------------------------------------------------
//    xxxx 0101 0101 xxxx 0101 0101 xxxx --- 0550550---PA[11:8] AND PA[5:2] SET AS OUTPUTS

	 //NO NEED FOR OUTPUTS??

GPIOA->MODER &= (0xFF00F00F); //inputs// column and row, PA[11:8,5:2] = 00////////////		RESETTING PINS, MAKING PA[11:8] OUTPUT PINS
GPIOA->MODER |= (0x00550000); //outputs// column, PA[11:8] = 01


// configure push-pull pins 
//GPIOA->OTYPER &= ~(0xFFC3); //set type of push-pull, PA[5:2] = 1 ///////test to see if this can be commented out
//GPIOA->OSPEEDR &= (0xFFFFF00F); //set bits to low speed, PA[5:2] = 00**/test to see if this can be commented out
GPIOA->PUPDR &= (0xFFFFF00F); //pull-reset// row, PA[5:2] = 00///////////////////////		RESETS PA[5:2] TO 00 SO I CAN REWRITE    ~0FF0 WOULD HAVE WORKED AS WELL
GPIOA->PUPDR |= (0x00000550); //pull-up// row, PA[5:2] = 01  ////////////////////////		SETS PINS [5:2] AS PULL UP

}


void PinSetup2() {
Setup();
// configure GPIOA pins 

GPIOA->MODER &= (0xFF00F00F); //inputs// column and row, PA[11:8,5:2] = 00//////////		REUSE OF SETTING UP PINS LIKE ABOVE, MAKING PA[5:2] OUTPUTS NOW
GPIOA->MODER |= (0x0550); //outputs// row, PA[5:2] = 01

///configure push-pull pins 

//GPIOA->OTYPER &= ~(0xF0FF); //set type of push-pull, PA[11:8] = 1 //////////////////		WHAT IS THIS FOR??----PUSH PULL WITH MOSSFET
//GPIOA->OSPEEDR &= (0xFF00FFFF);    //set bits to low speed, PA[11:8] = 00** ////////		OUTPUT SPEED
GPIOA->PUPDR &= (0xFF00FFFF); //pull-reset// row, PA[11:8] = 00 //////////////////////		RESETTING TO MAKE [11:8] PULL UP NOW
GPIOA->PUPDR |= (0x00550000); //pull-up// row, PA[11:8] = 01  
} 

////////////////////////////////////////////////////////////////////////////////////		WRITTEN JUST LIKE DR HUNGS EXAMPLE
/*---------------------------------------------------*/
/* initialize interrupts used in the program */
/* EXTI1 = external interrupt one */
/* EXTI2 = external interrupt two */
/*---------------------------------------------------*/
void InterruptSetup() { //maybe void in ()
/* Enable SYSCFG clock */
RCC->APB2ENR |= 0x01; //enable interrupt clock SYSCFG

/* configure port PA0 as input source of EXTI0 */
SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;//clear EXTI1 bit in config reg ~(0xF)
SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PB;//PB configuration in EXTI0

/* configure and enable EXTI0 as falling-edge triggered */
EXTI->FTSR1 |= 0x0001; //falling edge trigger enabled
EXTI->IMR1 |= 0x0001;     //enable (unmask) EXTI0
EXTI->PR1 = EXTI_PR1_PIF1;

/* Program NVIC to clear pending bit and enable EXTI0 */
NVIC_ClearPendingIRQ(EXTI0_IRQn); //Clear NVIC pending bit 
NVIC_EnableIRQ(EXTI0_IRQn); //enable IRQ 

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////			DELAY FUNCTIONS
void debounce() { //
for (i=0; i<15; i++) { //outer loop
for (j=0; j<40; j++) { //inner loop
n = j; //dummy operation for single-step test
} //do nothing
}
}

/*----------------------------------------------------------*/
/* delay function - do nothing for about 1 second */
/*----------------------------------------------------------*/
void delay() { //
for (i=0; i<15; i++) { //outer loop
for (j=0; j<10000; j++) { //inner loop *4000*
n = j; //dummy operation for single-step test
} //do nothing
}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------*/
/* count function - control counting of the timers */
/*----------------------------------------------------------*/
void count() {

/* clear unwanted values */
GPIOB->ODR &= 0xFF87; //mask PB[6:3] to 0 ///////////////////////////////////////////////////////////////////////			ODR HAS ONLY 16BITS, ONE TO EACH PIN. MASKING THE BITS FOR PINS 6-3

unsigned int mask; //value for the mask of count

/* counting logic */
counter++;
if (counter >= 10) { //if countA goes past 9 then set to 0
counter = 0;
}

mask = 0; //maskA is set to 0////////////////////////////////////////////////////////////////////////////////////			RESETTING MASK SO CAN PUSH THIS VALUE TO PINS 3-6. BC COUNTER HAS INCREMENTED
mask = counter << 3; //shift countA left by 5. //////////////////////////////////////////////////////////////////			SETTING LSB TO PIN 3, GOES TO PIN 6 BC ITS A 4 BIT VALUE
GPIOB->ODR |= mask; //mask PB[6:3] with count
}

/*----------------------------------------------------------*/
/* keypad function - find which button has been pressed */
/*----------------------------------------------------------*/
void keypad() {
button = 0;
rowNum = 0;
colNum = 0;

/* clear unwanted values */
GPIOB->ODR &= 0xFF87; //mask PB[6:3] to 0 /////////////////////////////////////////////////////////////////////			ODR HAS ONLY 16BITS, ONE TO EACH PIN. MASKING THE BITS FOR PINS 6-3
/* columns output 0 and find which row is 0*/
//PinSetup1();
row=0;
GPIOA->ODR &= (0xF0FF); //set column to output 0, PA[11:8] = 0

for(k=0; k<4; k++); //delay for values to load

row = (~GPIOA->IDR & 0x003C); //get row inputs, PA[5:2] = 0***check~
row = row >> 2; //shift right by 2

do {
	row = row << 1; //shift left by 1 to find row count
	rowNum++; //add to row count
	} 

while(row < 0x10) ; //can only shift four times



/* rows output 0 and find which column is 0 */
PinSetup2();
debounce(); 			///////////////////////////////////////////////////////////////////////////////////////			DELAY
col=0;
GPIOA->ODR &= (0xFFC3); //set row to output 0, PA[5:2] = 0
for(k=0; k<4; k++); //delay for values to load
col = (~GPIOA->IDR & 0xF00); //get column inputs, PA[11:8] = 0
col = col >> 8; //shift right by 8

do {
	col = col << 1; //shift left by 1 to find column count
	colNum++; //add to column count
	} 

while(col < 0x10) ; //can only shift four times

button = keypad_map[--rowNum][--colNum];    //test and see if works****
 
GPIOB->ODR = button << 3; //output button value, PB[6:3]
button = 0;
delay (); //1 sec delay
delay (); //1 sec delay
}

/*----------------------------------------------------------*/
/* display function - PB[6:3], count (1s) or button (3s) */
/*----------------------------------------------------------*/
//void display_button() {
// GPIOB->ODR = button << 3; //output button value, PB[6:3]
// delay (); //1 sec delay
// delay (); //1 sec delay
//}

/*----------------------------------------------------------*/
/* interrupt handler EXTI0 - keypad has been pressed */
/*----------------------------------------------------------*/
void EXTI0_IRQHandler() { //maybe put void in ()

//debounce();
go=go++;
keypad(); //keypad logic
//display_button(); //display button
PinSetup1();
debounce();
NVIC_ClearPendingIRQ(EXTI0_IRQn); //clear NVIC pending bit with EXTI line 1 interrupt
EXTI->PR1 |= 0x0001;                //clear EXTI0 pending bit*
__enable_irq(); //enable interupts* Maybe this has to go before above line
}

/*------------------------------------------------*/
/* main program */
/*------------------------------------------------*/
int main(void) {
Setup(); //configure clocks and GPIOB pins
PinSetup1();
InterruptSetup(); //configure interrupts
counter = 0; //initialize count
col = 0; //initialize col
row = 0; //initialize row
go = 1; //initialize go
/* Endless loop */
while (1) { //endless loop
delay(); //delay for 1 seconds
count(); //counting logic
//if(go != 1){ //see if button has been pressed
//keypad(); //keypad logic
//display_button(); //display button
//}
}/* repeat forever */
}
