/**
  ******************************************************************************
  * @file    PWM.c
  * @author  Lucas Schulze
  * @GitHub  /Schulze18/NUCLEO-F303K8/Examples-and-Applications
  * @version V1.0
  * @date    06-January-2018
  * @brief   This program is used to control a H-Bridge TB6612FNG. Both PWM use the
  *          same timer, TIM2. There is a function to change the period of the PWMs
  *          pulse. And a button is setup to change this value.  
  ******************************************************************************
*/

//Inclusão de Header
#include "stm32f30x.h"

//Constantes
#define PWM_PERIODO 			(7200000/17570)			//Frequencia irá ser 17,570 kHz pois o clock padrão é 72Mhz
#define TIMER_PWM				TIM2
#define BARRAMENTO_TIMER_PWM	RCC_APB1Periph_TIM2

//Pinos
#define PINO_PWMA       GPIO_Pin_2
#define PINO_SOURCE_PWMA	GPIO_PinSource2			//Necessário para a função GPIO_PinAFConfig ()
#define PINO_PWMA_AF		GPIO_AF_1
#define CANAL_PWMA 			3
#define INPUT_PWMA_1		GPIO_Pin_7
#define INPUT_PWMA_2		GPIO_Pin_6

#define PINO_PWMB			GPIO_Pin_3
#define PINO_SOURCE_PWMB	GPIO_PinSource3
#define PINO_PWMB_AF		GPIO_AF_1
#define CANAL_PWMB			4
#define INPUT_PWMB_1		GPIO_Pin_5
#define INPUT_PWMB_2		GPIO_Pin_4

//Ports
#define PORT_PWMA				  GPIOA
#define PORT_INPUT_PWA_1 	GPIOA
#define PORT_INPUT_PWA_2	GPIOA
#define PORT_PWMB			    GPIOA
#define PORT_INPUT_PWB_1	GPIOA
#define PORT_INPUT_PWB_2	GPIOA

//Protótipos de Funções
void PWM_setup(void);
void PWM(char pwm, int tempo_alto);				//Função de mudança do PWM, o valor do tempo em alto deve ser 0-255
void button_setup(void);

//Estruturas e Classes
TIM_OCInitTypeDef  TIM_OCInitStructure;

//Variaveis
int x;

//Main Functions
int main(void)
{
	//Habilita os Ports dos pinos
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);

	//Configura PWM
	PWM_setup();
	button_setup();
	x=-255;

	while (1)
	{
		//Lógica para o botão
		int y = (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0) );
 		if(y== 1){
			PWM('A',x);
			PWM('B',x);
			x=x+50;
		}

		//Garante que incremente uma vez apenas a variavel "x" ao apertar o botao
		while(y ==  1){
			y = (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0) );
		}
	}
}

/******************** Configuração do PWM **************************
 	 - Configuração dos Pinos
 	 - Configuração do TIMER
 	 - Configuração Output Compare com os tempos de pulso
 	 - Inicialização
  */
void PWM_setup(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	//Configuração dos pinos de entrada do motor
	GPIO_InitStructure.GPIO_Pin = INPUT_PWMA_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(PORT_INPUT_PWA_1,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = INPUT_PWMA_2;
	GPIO_Init(PORT_INPUT_PWA_2,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = INPUT_PWMB_1;
	GPIO_Init(PORT_INPUT_PWB_1,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = INPUT_PWMB_2;
	GPIO_Init(PORT_INPUT_PWB_2,&GPIO_InitStructure);
	GPIO_ResetBits(PORT_INPUT_PWA_1,INPUT_PWMA_1);
	GPIO_ResetBits(PORT_INPUT_PWA_2,INPUT_PWMA_2);
	GPIO_ResetBits(PORT_INPUT_PWB_1,INPUT_PWMB_1);
	GPIO_ResetBits(PORT_INPUT_PWB_2,INPUT_PWMB_2);

	// Configura os pinos do PWMA e PWMB com modo alternativo e push pull
	GPIO_InitStructure.GPIO_Pin = PINO_PWMA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(PORT_PWMA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = PINO_PWMB;
	GPIO_Init(PORT_PWMB, &GPIO_InitStructure);

	//Configuração Modo alternativo dos pinos
	GPIO_PinAFConfig(PORT_PWMA, PINO_SOURCE_PWMA, PINO_PWMA_AF); //GPIO_AF_6: refere-se a utilização com o TIM1
	GPIO_PinAFConfig(PORT_PWMB, PINO_SOURCE_PWMB, PINO_PWMB_AF); //GPIO_AF_1: refere-se a utilização com o TIM2, TIM3 ou TIM4

	//Habilita o barramento de clock do TIMER
	if(!(strcmp(TIMER_PWM,"TIM1")))RCC_APB2PeriphClockCmd(BARRAMENTO_TIMER_PWM,ENABLE);
	else RCC_APB1PeriphClockCmd(BARRAMENTO_TIMER_PWM, ENABLE);

	//Configuração Padrão do Timer
	TIM_TimeBaseStructure.TIM_Prescaler = 1;							//O clock do timer será 72Mhz/TIM_Prescaler
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			//Contagem crescente
	TIM_TimeBaseStructure.TIM_Period = PWM_PERIODO;						//Periodo do PWM
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIMER_PWM, &TIM_TimeBaseStructure);

	//Configuração do Outpur Compare
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;					//PWM1 e PWM2 possuem logica invertida, define se começa em alta ou em baixa
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;									//Tempo do pulso em alta, inicialmente zero
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

	//Inicialização do Output Compare para o PWMA
	if(CANAL_PWMA == 1) TIM_OC1Init(TIMER_PWM, &TIM_OCInitStructure);
	else if (CANAL_PWMA == 2) TIM_OC2Init(TIMER_PWM, &TIM_OCInitStructure);
	else if (CANAL_PWMA == 3) TIM_OC3Init(TIMER_PWM, &TIM_OCInitStructure);
	else if (CANAL_PWMA == 4) TIM_OC4Init(TIMER_PWM, &TIM_OCInitStructure);

	//Inicialização do Output Compare para o PWMB
	if(CANAL_PWMB == 1) TIM_OC1Init(TIMER_PWM, &TIM_OCInitStructure);
	else if (CANAL_PWMB == 2) TIM_OC2Init(TIMER_PWM, &TIM_OCInitStructure);
	else if (CANAL_PWMB == 3) TIM_OC3Init(TIMER_PWM, &TIM_OCInitStructure);
	else if (CANAL_PWMB == 4) TIM_OC4Init(TIMER_PWM, &TIM_OCInitStructure);

	//Habilita o Timer do PWM
	TIM_Cmd(TIMER_PWM,ENABLE);

	//Habilita o Preload do canal do PWMA
	if(CANAL_PWMA == 1)   TIM_OC1PreloadConfig(TIMER_PWM,TIM_OCPreload_Enable);
	else if (CANAL_PWMA == 2) TIM_OC2PreloadConfig(TIMER_PWM,TIM_OCPreload_Enable);
	else if (CANAL_PWMA == 3) TIM_OC3PreloadConfig(TIMER_PWM,TIM_OCPreload_Enable);
	else if (CANAL_PWMA == 4) TIM_OC4PreloadConfig(TIMER_PWM,TIM_OCPreload_Enable);

	//Habilita o Preload do canal do PWMB
	if(CANAL_PWMB == 1)   TIM_OC1PreloadConfig(TIMER_PWM,TIM_OCPreload_Enable);
	else if (CANAL_PWMB == 2) TIM_OC2PreloadConfig(TIMER_PWM,TIM_OCPreload_Enable);
	else if (CANAL_PWMB == 3) TIM_OC3PreloadConfig(TIMER_PWM,TIM_OCPreload_Enable);
	else if (CANAL_PWMB == 4) TIM_OC4PreloadConfig(TIMER_PWM,TIM_OCPreload_Enable);

}

void PWM(char pwm, int tempo_alto){
	if(pwm == 'A'){
		if (tempo_alto<0){
			tempo_alto= -tempo_alto;
			GPIO_SetBits(PORT_INPUT_PWA_1,INPUT_PWMA_1);
			GPIO_ResetBits(PORT_INPUT_PWA_2,INPUT_PWMA_2);
		}
		else{
			GPIO_ResetBits(PORT_INPUT_PWA_1,INPUT_PWMA_1);
			GPIO_SetBits(PORT_INPUT_PWA_2,INPUT_PWMA_2);
		}

		if(tempo_alto>255)tempo_alto=255;
		long int pulso_pwmA = (uint32_t)(((uint32_t)tempo_alto*PWM_PERIODO)/255);

		TIM_OCInitStructure.TIM_Pulse = pulso_pwmA;
		if(CANAL_PWMA == 1) TIM_OC1Init(TIMER_PWM, &TIM_OCInitStructure);
		else if (CANAL_PWMA == 2) TIM_OC2Init(TIMER_PWM, &TIM_OCInitStructure);
		else if (CANAL_PWMA == 3) TIM_OC3Init(TIMER_PWM, &TIM_OCInitStructure);
		else if (CANAL_PWMA == 4) TIM_OC4Init(TIMER_PWM, &TIM_OCInitStructure);
	}
	else if(pwm=='B'){
		if (tempo_alto<0){
			tempo_alto= -tempo_alto;
			GPIO_SetBits(PORT_INPUT_PWB_1,INPUT_PWMB_1);
			GPIO_ResetBits(PORT_INPUT_PWB_2,INPUT_PWMB_2);
		}
		else{
			GPIO_ResetBits(PORT_INPUT_PWB_1,INPUT_PWMB_1);
			GPIO_SetBits(PORT_INPUT_PWB_2,INPUT_PWMB_2);
		}

		if(tempo_alto>255)tempo_alto=255;
		long int pulso_pwmB = (uint32_t)(((uint32_t)tempo_alto*PWM_PERIODO)/255);
		TIM_OCInitStructure.TIM_Pulse = pulso_pwmB;

		if(CANAL_PWMB == 1) TIM_OC1Init(TIMER_PWM, &TIM_OCInitStructure);
		else if (CANAL_PWMB == 2) TIM_OC2Init(TIMER_PWM, &TIM_OCInitStructure);
		else if (CANAL_PWMB == 3) TIM_OC3Init(TIMER_PWM, &TIM_OCInitStructure);
		else if (CANAL_PWMB == 4) TIM_OC4Init(TIMER_PWM, &TIM_OCInitStructure);
	}
}

void button_setup(){
	GPIO_InitTypeDef botao;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	botao.GPIO_Pin = GPIO_Pin_0;
	botao.GPIO_Mode = GPIO_Mode_IN;
	botao.GPIO_PuPd = GPIO_PuPd_NOPULL;
	botao.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &botao);
}
