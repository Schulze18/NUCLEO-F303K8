/**
  ******************************************************************************
  * @file    Interruption_10kHz.c
  * @author  Lucas Schulze
  * @GitHub  /Schulze18/NUCLEO-F303K8/Examples-and-Applications
  * @version V1.0
  * @date    06-January-2018
  * @brief   In this program a interruption happens at a 10kHz frequency.  
  ******************************************************************************
*/

#include "stm32f30x.h"

//Constantes
#define TIMER_PERIODO		1000

//Prototipo de Funções
void LedSetup(void);
void TIM3_Setup(void);

//Criação de Variaveis
uint32_t x=0;
uint32_t contador=0,y=0;
int led=0;

int main(void)
{
	LedSetup();
	TIM3_Setup();
	while(1){
	}
}

void LedSetup(void){
	GPIO_InitTypeDef led13;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	led13.GPIO_Pin = GPIO_Pin_11;
	led13.GPIO_Mode = GPIO_Mode_OUT;
	led13.GPIO_OType = GPIO_OType_PP;
	led13.GPIO_PuPd = GPIO_PuPd_NOPULL;
	led13.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &led13);
	GPIO_ResetBits(GPIOA, GPIO_Pin_11);
}

void TIM3_Setup(void){
	uint16_t PrescalerValue = 0;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	//Habilita a interrupção no EXTI0 e configura sua prioridade
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;             //Mesma função de tratamento de Interrupção para todos os EXTI de 5 a 9
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//TIM3 clock enable 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	//Testar apenas com interrupção no TIMER 3
	TIM_TimeBaseStructure.TIM_Period = 20;//(7200000/17570)
	TIM_TimeBaseStructure.TIM_Prescaler = 35;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM3,ENABLE);
	}

void TIM3_IRQHandler(){
	//Para utilizar o mesmo timer para mais de uma aplicação, basta ter mais variaveis como "contador"
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!= RESET)
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		if(x==1){
			GPIO_SetBits(GPIOA,GPIO_Pin_11);
			x=0;
		}
		else{
			GPIO_ResetBits(GPIOA,GPIO_Pin_11);
			x=1;
		}
	}
}
