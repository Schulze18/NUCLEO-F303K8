/**
  ******************************************************************************
  * @file    SPI_MCP3008.c
  * @author  Lucas Schulze
  * @GitHub  /Schulze18/NUCLEO-F303K8/Examples-and-Applications
  * @version V1.0
  * @date    06-January-2018
  * @brief   This program is used to get the data from the 8 channels of MCP3008
             by SPI. 
  ******************************************************************************
*/

//Inclusão de Header
#include "stm32f30x.h"

//Constantes
#define	CANAL_STOP	0x80
#define CANAL_S1	0x90
#define CANAL_S2	0xA0
#define CANAL_S3	0xB0
#define CANAL_S4	0xC0
#define CANAL_S5	0xD0
#define CANAL_S6	0xE0
#define CANAL_CURVA	0xF0
#define SPI_BAUDRATE	SPI_BaudRatePrescaler_32
#define BARRAMENTO_SPI	RCC_APB2Periph_SPI1

//Pinos
#define PINO_SPI_SELETOR	GPIO_Pin_8

#define PINO_SPI_SCK		GPIO_Pin_3
#define PINO_SOURCE_SPI_SCK	GPIO_PinSource3
#define PINO_SPI_SCK_AF		GPIO_AF_5

#define PINO_SPI_MISO		GPIO_Pin_4
#define PINO_SOURCE_SPI_MISO	GPIO_PinSource4
#define PINO_SPI_MISO_AF	GPIO_AF_5

#define PINO_SPI_MOSI		GPIO_Pin_5
#define PINO_SOURCE_SPI_MOSI	GPIO_PinSource5
#define PINO_SPI_MOSI_AF	GPIO_AF_5

//Ports
#define PORT_SPI_SELETOR	GPIOA
#define PORT_SPI_SCK		GPIOB
#define PORT_SPI_MISO		GPIOB
#define PORT_SPI_MOSI           GPIOB

//Protótipos de Funções
void SPI_config(void);
int leitura_SPI(int);
void led_config(void);				//Função apenas para teste
void SPI_interrupcao(void);			//Função que executa toda a leitura dos sensores


//Variaveis
int sensores_linha[6];
int sensor_stop;
int sensor_curva;

int main(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	SPI_config();
	led_config();
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);

	while(1){
		SPI_interrupcao();
		//Lógica para acender o led

		if (sensor_stop>0)
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_1);
		}
		else {
			GPIO_ResetBits(GPIOB,GPIO_Pin_1);
		}
 	}
}


void SPI_config(void)
{
	//Crianção de estruturas
	SPI_InitTypeDef SPI_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitTypeDef seletor;

	RCC_APB2PeriphClockCmd(BARRAMENTO_SPI, ENABLE);
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BAUDRATE;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
	SPI_Init(SPI1, &SPI_InitStruct);
	SPI_Cmd(SPI1, ENABLE);

	//Configuração pinos SCK, MOSI e MISO
	GPIO_InitStruct.GPIO_Pin = PINO_SPI_SCK;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PORT_SPI_SCK, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = PINO_SPI_MISO;
	GPIO_Init(PORT_SPI_MISO, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = PINO_SPI_MOSI;
	GPIO_Init(PORT_SPI_MOSI, &GPIO_InitStruct);

	//Configura Modo Alternativo Pinos
	GPIO_PinAFConfig(PORT_SPI_SCK, PINO_SOURCE_SPI_SCK, PINO_SPI_SCK_AF );      //SCK
	GPIO_PinAFConfig(PORT_SPI_MISO, PINO_SOURCE_SPI_MISO, PINO_SPI_MISO_AF );   //MISO
	GPIO_PinAFConfig(PORT_SPI_MOSI, PINO_SOURCE_SPI_MOSI, PINO_SPI_MOSI_AF );   //MOSI

	//Configura a chave de seleção 
	seletor.GPIO_Pin = PINO_SPI_SELETOR;
	seletor.GPIO_Mode = GPIO_Mode_OUT;
	seletor.GPIO_OType = GPIO_OType_PP;
	seletor.GPIO_PuPd = GPIO_PuPd_UP;
	seletor.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PORT_SPI_SELETOR, &seletor);

	//Garante que o MCP fique em standby
	GPIO_ResetBits(PORT_SPI_SELETOR,PINO_SPI_SELETOR);
	GPIO_SetBits(PORT_SPI_SELETOR,PINO_SPI_SELETOR);

}

//Function to get data from MCP by SPI
int leitura_SPI( int dado){
	 uint8_t result;

	 SPI_SendData8(SPI1, dado);
	 while (!(SPI1->SR & (SPI_I2S_FLAG_TXE)));		//Verifica se o Bite já foi enviado
	 while (SPI1->SR & (SPI_I2S_FLAG_BSY));			//Verifica se o SPI esta livre
	 result = SPI_ReceiveData8(SPI1);			//Salva o Byte recebido

	 return result;
}

void led_config(void){
	GPIO_InitTypeDef led_9;

	led_9.GPIO_Pin = GPIO_Pin_1;
	led_9.GPIO_Mode = GPIO_Mode_OUT;
	led_9.GPIO_OType = GPIO_OType_PP;
	led_9.GPIO_PuPd = GPIO_PuPd_NOPULL;
	led_9.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &led_9);

}

//Function to get the data from all the 8 channels of MCP3008 
void SPI_interrupcao(void){
	int k=0x01; 															              //Valor de Start para a comunicação
	long int x1,x2,x3;

	//Leitura Canal 0 - Sensor Stop
	GPIO_ResetBits(PORT_SPI_SELETOR,PINO_SPI_SELETOR);		//Habilita o MCP3008 (É necessário a cada nova troca de informações)
	x1 = leitura_SPI(k);
	x2 = leitura_SPI(CANAL_STOP);
	x3 = leitura_SPI(k);					        //É necessário enviar qualquer informação, apenas para receber os últimos 8 bits
	sensor_stop = ((x2 & 0x03) << 8) | (x3 & 0x00FF);		//Lógica para unir os 2 bits MSB com os 8 bits LSB
	GPIO_SetBits(PORT_SPI_SELETOR,PINO_SPI_SELETOR);		//Desabilita o MCP3008

	//Leitura Canal 1 - Sensor Linha 1
	GPIO_ResetBits(PORT_SPI_SELETOR,PINO_SPI_SELETOR);
	x1 = leitura_SPI(k);
	x2 = leitura_SPI(CANAL_S1);
	x3 = leitura_SPI(k);
	sensores_linha[0] = ((x2 & 0x03) << 8) | (x3 & 0x00FF);
	GPIO_SetBits(PORT_SPI_SELETOR,PINO_SPI_SELETOR);

	//Leitura Canal 2 - Sensor Linha 2
	GPIO_ResetBits(PORT_SPI_SELETOR,PINO_SPI_SELETOR);
	x1 = leitura_SPI(k);
	x2 = leitura_SPI(CANAL_S2);
	x3 = leitura_SPI(k);
	sensores_linha[1] = ((x2 & 0x03) << 8) | (x3 & 0x00FF);
	GPIO_SetBits(PORT_SPI_SELETOR,PINO_SPI_SELETOR);

	//Leitura Canal 3 - Sensor Linha 3
	GPIO_ResetBits(PORT_SPI_SELETOR,PINO_SPI_SELETOR);
	x1 = leitura_SPI(k);
	x2 = leitura_SPI(CANAL_S3);
	x3 = leitura_SPI(k);
	sensores_linha[2] = ((x2 & 0x03) << 8) | (x3 & 0x00FF);
	GPIO_SetBits(PORT_SPI_SELETOR,PINO_SPI_SELETOR);

	//Leitura Canal 4 - Sensor Linha 4
	GPIO_ResetBits(PORT_SPI_SELETOR,PINO_SPI_SELETOR);
	x1 = leitura_SPI(k);
	x2 = leitura_SPI(CANAL_S4);
	x3 = leitura_SPI(k);
	sensores_linha[3] = ((x2 & 0x03) << 8) | (x3 & 0x00FF);
	GPIO_SetBits(PORT_SPI_SELETOR,PINO_SPI_SELETOR);

	//Leitura Canal 5 - Sensor Linha 5
	GPIO_ResetBits(PORT_SPI_SELETOR,PINO_SPI_SELETOR);
	x1 = leitura_SPI(k);
	x2 = leitura_SPI(CANAL_S5);
	x3 = leitura_SPI(k);
	sensores_linha[4] = ((x2 & 0x03) << 8) | (x3 & 0x00FF);
	GPIO_SetBits(PORT_SPI_SELETOR,PINO_SPI_SELETOR);

	//Leitura Canal 6 - Sensor Linha 6
	GPIO_ResetBits(PORT_SPI_SELETOR,PINO_SPI_SELETOR);
	x1 = leitura_SPI(k);
	x2 = leitura_SPI(CANAL_S6);
	x3 = leitura_SPI(k);
	sensores_linha[5] = ((x2 & 0x03) << 8) | (x3 & 0x00FF);
	GPIO_SetBits(PORT_SPI_SELETOR,PINO_SPI_SELETOR);

	//Leitura Canal 7 - Sensor Curva
	GPIO_ResetBits(PORT_SPI_SELETOR,PINO_SPI_SELETOR);
	x1 = leitura_SPI(k);
	x2 = leitura_SPI(CANAL_CURVA);
	x3 = leitura_SPI(k);
	sensor_curva = ((x2 & 0x03) << 8) | (x3 & 0x00FF);
	GPIO_SetBits(PORT_SPI_SELETOR,PINO_SPI_SELETOR);
}

