/**
  ******************************************************************************
  * @file    Button.c
  * @author  Lucas Schulze
  * @GitHub  /Schulze18/NUCLEO-F303K8
  * @version V1.0
  * @date    15-October-2017
  * @brief   Configure a button and turn on/off a led as the button is pressed or not
  * 		 in STM32 Nucleo F303K8 with System Workbench for STM32
  ******************************************************************************
*/

#include "stm32f30x.h"
			
//Port and Pin of LED
#define PIN_LED		GPIO_Pin_3
#define PORT_LED	GPIOB
//Port and Pin of Button
#define PIN_BUTTON	GPIO_Pin_6
#define	PORT_BUTTON	GPIOB

//Function prototype
void LED_setup(void);
void BUTTON_setup(void);

int main(void)
{
	//Enable Clock from LEDs and Buttons Port
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE);

	//Call function to configure LEDs Pin
	LED_setup();
	BUTTON_setup();

	//Main Loop
	while (1){
		//Turn On LED if the Button is pressed
		if(GPIO_ReadInputDataBit(PORT_BUTTON,PIN_BUTTON)==1)GPIO_SetBits(PORT_LED, PIN_LED);
		//Turn Off LED if the Button is released
		else GPIO_ResetBits(PORT_LED, PIN_LED);
	}
}

//Function to configure LEDs Pin
void LED_setup(void){
	//Create GPIO Init Structure for  LED
	GPIO_InitTypeDef led;

	led.GPIO_Pin = PIN_LED;
	led.GPIO_Mode = GPIO_Mode_OUT;
	led.GPIO_OType = GPIO_OType_PP;
	led.GPIO_PuPd = GPIO_PuPd_NOPULL;
	led.GPIO_Speed = GPIO_Speed_50MHz;

	//Initialize LEDs structure
	GPIO_Init(PORT_LED, &led);

}

//Function to configure Buttons Pin
void BUTTON_setup(void){
	//Create GPIO Init Structure for Button
	GPIO_InitTypeDef button;

	button.GPIO_Pin = PIN_BUTTON;
	button.GPIO_Mode = GPIO_Mode_IN;
	button.GPIO_OType = GPIO_OType_PP;
	button.GPIO_PuPd = GPIO_PuPd_NOPULL;
	button.GPIO_Speed = GPIO_Speed_50MHz;

	//Initialize Buttons structure
	GPIO_Init(PORT_BUTTON, &button);

}
