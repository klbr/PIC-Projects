#include <16F874.h>
#include "SpeedBump.h"
#use delay (clock=1000000)
#fuses XT,NOWDT,NOPUT
/************************************************/ 

long int count = 0; //conta os décimos de segundo
int localSpeed = 0; //armazena a velocidade em km/h
int altern = 0; //alternador usado para correção na contagem dos décimos de segundos
int vtimer = 0; //timer do tempo para calculo da velocidade
int vtimer2 = 0; //timer usado incrementar os segundos
int seconds = 0; //calcula os segundos

void initState()
{
	//definicoes do PIC
    setup_timer_0(RTCC_INTERNAL | RTCC_DIV_16); 
    //set_timer0(131);
	enable_interrupts(GLOBAL | INT_TIMER0 | INT_RB);
    set_timer0(131);	
	output_c(DEZENA[0] | UNIDADE[0]);
}

float getSpeed(long int ms)
{
	//inicialmente em m/s
	float speed = 0.0;
	if (ms > 0) speed = (40000.0 / ((float)ms * 100.0) );		
	//conversão para km/h
	speed = speed * 3.6;
	return speed;
}

#int_timer0
void trata_timer0() 
{ 
    set_timer0(131 + get_timer0());    
	vtimer++;
	vtimer2++;
    if (vtimer >= 12 && altern == 0)
	{
		count++;
		vtimer = 0;
		altern = 1;
    }
	else if (vtimer >= 13)
	{
		count++;
		vtimer = 0;
		altern = 0;
	}
	if (vtimer2 == 125)
	{
		seconds++;
		vtimer2 = 0;
	}
} 

#int_rb
void trata_rb()
{
	int x;
	//representa o Laço Indutivo 01
	//zera os contadores para iniciar uma nova contagem de tempo 
	if (input(START_SB) == 0)
	{		
		count = 0;		
		localSpeed = 0;		
	}
	//representa o Laço Indutivo 02
	//Exibe a velocidade e dispara a foto caso a mesma seja maior que o máximo permitido
	if (input(FINISH_SB) == 0)
	{		
		seconds = 0;
		localSpeed = (int)getSpeed(count);
		if (localSpeed > MAX_LIMIT_SPEED)
		{
			output_high(FLASH_PHOTO);
		}		
	}
	x = input_b();
}

void showSpeed()
{
	int dec = 0;
	int unt = 0;				
	if (localSpeed >= 99)
	{
		dec = 9;
		unt = 9;
	}
	else
	{
		dec = localSpeed / 10;
		//simula o MOD
		unt = localSpeed - (dec * 10);		
	}
	output_c(DEZENA[dec] | UNIDADE[unt]);
	//zera a velocidade local
	localSpeed = 0;
}

void start()
{
	while(TRUE)
	{
		//O Flash dura apenas 1 segundo
		if (seconds > 0 && seconds < 2)
		{
			output_low(FLASH_PHOTO);				
		}
		//Limpa a informação dos displays depois de 3 segundos
		if (seconds > 2)
		{			
			seconds = 0;	
			output_c(DEZENA[0] | UNIDADE[0]);	
			output_low(FLASH_PHOTO);				
		}
		if (localSpeed > 0)
		{		    
			showSpeed();
		}
	}
}

void main()
{
	initState();
	start();
}
