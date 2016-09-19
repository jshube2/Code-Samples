/*
Author: Joshua Shubert                                                                      
Version: 1.0.0                                                                             
Last Updated: 12-5-2015   
Description: This is the code for a HCS12 Microcontroller to implement a White Noise Generator.
The basic premise is the microcontroller will activate a fan to generate background noise when the
light intensity on a light sensor falls below a certain threshold. This is for people who need
some sort of background noise in order to fall asleep. It turns on when the lights go off in the
room and turns back on when the sun shines through your window or you turn the lights back on. 
Additionally a qualitative description of the light level is output to an LCD display.
Notes: This code should be compiled and loaded onto the HCS12 through CodeWarriors IDE or any other
IDE properly set up for the HCS12.
*/                                                                         
/*************************************************************************************************/ 
#include <hidef.h> /* common defines and macros */
#include "derivative.h" /* derivative-specific definitions */
#define IOREGS_BASE 0x0000
#define _IO8(off) *(unsigned char volatile *)(IOREGS_BASE + off) //define form prototype 8-bit
#define _IO16(off) *(unsigned short volatile *)(IOREGS_BASE + off) //define form prototype 16-bit
#define PORTT _IO8(0x240) 
#define PORTTi _IO8(0x241) // portT data register
#define IRQCR _IO8(0x001E)
#define PORTM _IO8(0x250) // portM data register
#define MCCTL _IO8(0x66) //modulus down conunter control
#define MCFLG _IO8(0x67) //down counter flags
#define MCCNT _IO16(0x76) //modulus down counter register
#define RCK 0x08
#define RS 0x01
#define ENABLE 0x02 
#define PLLSEL 0x80
#define LOCK 0x08
#define TFFCA 0x10
#define MCZF 0x80
#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04 
#define BIT3 0x08
/*************************************************************************************************/
/*                                                                                               */
/*                                   Function Prototypes                                         */
/*                                                                                               */
/*************************************************************************************************/ 
void SetClk8(void); //set up function
void MSDelay(int k); //milisecond delay function
void Command(char a); //Command to LCD function
void Print(char a); //Print to LCD function
void InitLCD(void); //Start LCD function
int  getLight(void); // gets the light value
void InitPWM(int,int,int); // Initializes the PWM to the transistor which controls the fan
/*************************************************************************************************/
/*                                                                                               */
/*                                Necessary Global Variables                                     */
/*                                                                                               */
/*************************************************************************************************/
int  DelayFlag=0;
int  CountDelay=0; //global delay counter 
int  Input;                        
/*************************************************************************************************/
/*                                                                                               */
/*                                     Begin Main() Function                                     */
/*                                                                                               */
/*************************************************************************************************/                                                        
void main(void) {
/*--------------------------------------Variable Declarations------------------------------------*/   
// Pre packaged strings to be output to the LCD display 
  char  str1[16] =   {'F','a','n',' ','O','n',' ','W','h','e','n',' ','D','a','r','k'}; 
  char  str2[16] =   {'L','I','G','H','T',':','D','a','r','k',' ',' ',' ',' ',' ',' '};
  char  str3[16] =   {'L','I','G','H','T',':','M','e','d','D','a','r','k',' ',' ',' '};
  char  str4[16] =   {'L','I','G','H','T',':','M','e','d','i','u','m',' ',' ',' ',' '};
  char  str5[16] =   {'L','I','G','H','T',':','M','e','d','B','r','i','g','h','t',' '};
  char  str6[16] =   {'L','I','G','H','T',':','B','r','i','g','h','t',' ',' ',' ',' '};  
  int i = 0;
  int prevLightVal;
  int lightV;
/*--------------------------------------Interface Set Up-----------------------------------------*/  
  SetClk8();
  DDRM = 0x3b;
  DDRT = 0xf0;
  DDRA = 0xfc; //make a0 and a1 inputs
  SPICR1=0x50;
  SPICR2 = 0x00;
  SPIBR=0x22;
  PORTM =PORTM | RCK;
  InitLCD();
  ATDCTL2 =0x80;      //turn on ATD power
  MSDelay(1);
/*--------------------------------------Menu Loop------------------------------------------------*/
  Main_Menu: 
  Command(0x01);
  for(i=0; i<17; i++) {
          Print(str1[i]);  
        }
/*------------------------------------------Light Detector---------------------------------------*/
  prevLightVal = 0;      
  while(1){ // Could probably compress this to a repeating function call for code clarity
    Command(0x01);
    lightV = getLight();
    if(lightV >= 0 & lightV < 150 & prevLightVal !=1) {
        Command(0xC0);
        prevLightVal = 1;
        for(i=0; i<17; i++)
          Print(str2[i]); 
        InitPWM(0,0,0); 
    }
    if(lightV >= 150 & lightV < 200 & prevLightVal !=2) {
        Command(0xC0);
        prevLightVal = 2;    
        for(i=0; i<17; i++) 
          Print(str3[i]);
        InitPWM(0,0,0);  
    }
    if(lightV >= 200 & lightV < 220 & prevLightVal !=3) {
        Command(0x01);
        prevLightVal = 3;
        for(i=0; i<17; i++)
          Print(str4[i]); 
        InitPWM(0,0,0);
    }
    if(lightV >= 220 & lightV < 240 & prevLightVal !=4) {
        Command(0xC0);
        prevLightVal = 4;    
        for(i=0; i<17; i++) 
          Print(str5[i]);  
        InitPWM(100,100,0x01);
    }
    if(lightV >= 240 & lightV < 255 & prevLightVal !=5) {
        Command(0xC0);
        prevLightVal = 5;    
        for(i=0; i<17; i++) 
          Print(str6[i]); 
        InitPWM(100,100,0x01); 
    }
  }
EnableInterrupts;
}
/*--------------------------------------End of Main----------------------------------------------*/

/*************************************************************************************************/
/*                                                                                               */
/*                                     Function Definitions                                      */
/*                                                                                               */
/*************************************************************************************************/
void InitPWM(int PER, int DUTY, int CHAN) 
{
    PWMCLK = 0; /* select clock A as the clock source */
    PWMPOL = 1; /* make waveform to start with high level */
    PWMCTL = 0x0C; /* select 8-bit mode */
    PWMPRCLK = 2; /* set clock A prescaler to 4 */
    PWMCAE = 0; /* select left-aligned mode */
    PWMPER0 = PER; /* set period of PWM0 to 0.1 ms */
    PWMDTY0 = DUTY; /* set duty cycle to 100% */
    PWME |= CHAN; /* enable PWM0 channel */
}
void InitLCD(void)
{
    MSDelay(10);
    Command(0x30);
    MSDelay(10);
    Command(0x30);
    MSDelay(10);
    Command(0x30);
    MSDelay(10);
    Command(0x38);
    MSDelay(10);
    Command(0x0C); //starts LCD
    MSDelay(10);
    Command(0x01); //clears LCD
    MSDelay(30); 
}
void Command(char a) //Send a command for LCD
{
    while(!(SPISR & 0x20)); //Wait for register empty flag (SPIEF) ; spi_b: BRCLR SPSR,$20,spi_b
    SPIDR = a; //Output command via SPI to SIPO ; STAA SPDR
    while(!(SPISR & 0x80)); //Wait for SPI Flag ; CKFLG2 BRCLR SPSR,$80,CKFLG2
    a = SPIDR; //Equate a with SPIDR ; LDAA SPDR
    asm(nop); //Wait for 1 cycle ; NOP
    PORTM = PORTM & ~RCK; //Pulse RCK = $08 ; BCLR PortM, RCK
    asm(nop); //Wait for 1 cycle ; NOP
    asm(nop); //Wait for 1 cycle ; NOP
    PORTM = PORTM | RCK; //Data Ready for LCD ; BSET PortM, RCK
    PORTM = PORTM & ~RS; //RS = $00 
    asm(nop); //Wait for 1 cycle ; NOP
    asm(nop); //Wait for 1 cycle ; NOP
    asm(nop); //Wait for 1 cycle ; NOP
    PORTM = PORTM | ENABLE; //Fire ENABLE = $02 ; BSET PortM, ENABLE
    asm(nop); //Wait for 1 cycle ; NOP
    asm(nop); //Wait for 1 cycle ; NOP
    PORTM = PORTM & ~ENABLE; //ENABLE off ; BCLR PortM, ENABLE
    MSDelay(1); //Delay ; JSR delay
}
void Print(char a) // Print a character to the LCD;
{
    while(!(SPISR & 0x20)); //Wait for register empty flag (SPIEF) ; spi_b: BRCLR SPSR,$20,spi_b
    SPIDR = a; //Output command via SPI to SIPO ; STAA SPDR
    while(!(SPISR & 0x80)); //Wait for SPI Flag ; CKFLG2 BRCLR SPSR,$80,CKFLG2
    Input = SPIDR; 
    asm(nop); //Wait for 1 cycle ; NOP
    PORTM = PORTM & ~RCK; //Pulse RCK = $08 ; BCLR PortM, RCK
    asm(nop); //Wait for 1 cycle ; NOP
    asm(nop); //Wait for 1 cycle ; NOP
    PORTM = PORTM | RCK; //Data Ready for LCD ; BSET PortM, RCK
    PORTM = PORTM | RS; //RS = $01 ; BSET PortM, RS
    asm(nop); //Wait for 1 cycle ; NOP
    asm(nop); //Wait for 1 cycle ; NOP
    asm(nop); //Wait for 1 cycle ; NOP
    PORTM = PORTM | ENABLE; //Fire ENABLE = $02 ; BSET PortM, ENABLE
    asm(nop); //Wait for 1 cycle ; NOP
    asm(nop); //Wait for 1 cycle ; NOP
    PORTM = PORTM & ~ENABLE; //ENABLE off ; BCLR PortM, ENABLE
    MSDelay(1); //Delay ; JSR delay
}
void SetClk8(void) 
{   // Set up the internal clock of the HCS12 micro controller
    asm(sei); // turn of interrupts
    CLKSEL &= PLLSEL; // disengage PLL from system
    SYNR = 0x02; // set SYSCLK to 24 MHz from a 4-MHz oscillator
    REFDV = 0; // 
    PLLCTL = 0x40; // turn on PLL, set automatic 
    while(!(CRGFLG & LOCK)); // wait for HIGN on LOCK bit at address CRGFLG
    asm(nop); // very short delays
    asm(nop);
    CLKSEL |= PLLSEL; // clock derived from PLL
    TSCR1 = 0x90; /* enable TCNT and fast timer flag clear */
    TSCR2 = 0x06; /* disable timer interrupt, set prescaler to 64 */
    TIOS = 0x01; /* enable OC0 */
    TIE = 0x01;
    asm(cli); // turn on interrups
}
void MSDelay(int k)
{
    DelayFlag=1; //flag for to count delays by mili second
    while(CountDelay<k); //stays here until DelayCount==k 
    CountDelay=0; //resets delay counter
    DelayFlag=0; //resets delay flag
} 
int getLight(void) 
{  // Get digital light value from internal A/D Converter
   ATDCTL3 =0x08;           //taking one sample
   ATDCTL4 =0x85;           //10 bit at 2MHz 
   ATDCTL5 =0x80;           //convert, right justified, unsigned, single scan of channel 2 
   while(ATDSTAT0 & 0x80); //wait for the sequence complete flag   
   return ATDDR0; 
}