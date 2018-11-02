// Lab8.c
// Runs on LM4F120 or TM4C123
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
// Last Modified: 4/5/2016 

// Analog Input connected to PE2=ADC1
// displays on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats

#include <stdint.h>

#include "ST7735.h"
#include "TExaS.h"
#include "ADC.h"
#include "print.h"
#include "tm4c123gh6pm.h"

//*****the first three main programs are for debugging *****
// main1 tests just the ADC and slide pot, use debugger to see data
// main2 adds the LCD to the ADC and slide pot, ADC data is on Nokia
// main3 adds your convert function, position data is no Nokia

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
// Initialize Port F so PF1, PF2 and PF3 are heartbeats
volatile uint32_t delay2 = 0;
	uint32_t ADCMail = 0;
	uint32_t ADCStatus = 0;


void PortAF_Init(void){
	 SYSCTL_RCGC2_R |= 0x01; // activate port A
  while((SYSCTL_RCGC2_R&0x10) == 0){}; // allow time for clock to start

  // toggle RST low to reset; CS low so it'll listen to us
  // SSI0Fss is temporarily used as GPIO
  GPIO_PORTA_DIR_R |= 0xC8;             // make PA3,6,7 out
  GPIO_PORTA_AFSEL_R &= ~0xC8;          // disable alt funct on PA3,6,7
  GPIO_PORTA_DEN_R |= 0xC8;    

		
		SYSCTL_RCGC2_R |= 0x20;
	while((SYSCTL_RCGC2_R&0x10) == 0){};
	delay2 =1;
	delay2 =2;
	GPIO_PORTF_DIR_R |= 0x06;
	GPIO_PORTF_DEN_R |= 0x06;
	GPIO_PORTF_AFSEL_R &= ~0x06;
}
uint32_t Data;        // 12-bit ADC
uint32_t Position;    // 32-bit fixed-point 0.001 cm


//uint16_t Convert_2_real(uint16_t num){
//	uint16_t result;
//	result = ((445*num)-15538)/100;		//NOT THE REAL THING
//	return result;
//}

void SysTick_Init(void){
	  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = 2000000;	// reload value	--->	12.5ns*(clock cycles) = .025s
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
                              // enable SysTick with core clock and interrupts
  NVIC_ST_CTRL_R = 0x0007;
	//ADCStatus = 1;
}

void SysTick_Handler(void){
	GPIO_PORTF_DATA_R ^= 0x06;
	GPIO_PORTF_DATA_R ^= 0x06;
	ADCMail = ADC_In();
	ADCStatus  = 1;
	GPIO_PORTF_DATA_R ^= 0x06;
}






//int main1(void){      // single step this program and look at Data
//  TExaS_Init();       // Bus clock is 80 MHz 
//  ADC_Init();         // turn on ADC, set channel to 1
//  while(1){                
//    Data = ADC_In();  // sample 12-bit channel 1
//  }
//}

//int main2(void){
//  TExaS_Init();       // Bus clock is 80 MHz 
//  ADC_Init();         // turn on ADC, set channel to 1
//  ST7735_InitR(INITR_REDTAB); 
//								//ST7735_OutString("Lab 7!\nWelcome to EE319K");
//  PortAF_Init();
//								//ST7735_OutString("Lab 7!\nWelcome to EE319K");
//  while(1){           // use scope to measure execution time for ADC_In and LCD_OutDec           
//    PF2 = 0x04;       // Profile ADC
//    Data = ADC_In();  // sample 12-bit channel 1
//    PF2 = 0x00;       // end of ADC Profile
//    ST7735_SetCursor(0,0);
//    PF1 = 0x02;       // Profile LCD
//    LCD_OutDec(Data); 
//    ST7735_OutString("    ");  // these spaces are used to coverup characters from last output
//    PF1 = 0;          // end of LCD Profile
//  }
//}


uint32_t Convert(uint32_t input){
	uint32_t result = 0;
	result = ((input*461)+36899)/1000;
  return result;
}


//int main3(void){ 
//  TExaS_Init();         // Bus clock is 80 MHz 
//  ST7735_InitR(INITR_REDTAB); 
//  ADC_Init();  
//  PortAF_Init();	// turn on ADC, set channel to 1
//  while(1){  
//    PF2 ^= 0x04;      // Heartbeat
//    Data = ADC_In();  // sample 12-bit channel 1
//    PF3 = 0x08;       // Profile Convert
//    Position = Convert(Data); 
//    PF3 = 0;          // end of Convert Profile
//    PF1 = 0x02;       // Profile LCD
//    ST7735_SetCursor(0,0);
//    LCD_OutDec(Data); 
//		ST7735_OutString("    "); 
//    ST7735_SetCursor(6,0);
//    LCD_OutFix(Position);
//    PF1 = 0;          // end of LCD Profile
//  }
//}   
int main(void){
	uint32_t temp = 0;
	TExaS_Init();         // Bus clock is 80 MHz 
  ST7735_InitR(INITR_REDTAB); 
  ADC_Init();  
  PortAF_Init();  
	SysTick_Init();					// turn on ADC, set channel to 1
  while(1){    
		while (ADCStatus == 0){}
    ADCMail = ADC_In();  // sample 12-bit channel 1
		ADCStatus = 0; 
		temp = Convert(ADCMail);
		ST7735_SetCursor(0,0);
		ST7735_OutString("    "); 
		LCD_OutFix(temp);
	}
}

