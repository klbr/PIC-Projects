#include <16F874.h>
#include "ParkingSystem.h"
#use delay (clock=1000000)
#fuses XT,NOWDT,NOPUT
/************************************************/ 
long int Input = 0;
long int Output = 0;
long int Total = 0;

int ActiveIn = 0;
int ActiveOut = 0;

//int vtimer2 = 0;
//long int secondsIn = 0;
//long int secondsOut = 0;
		
void initState()
{
	//definicoes do PIC
    setup_timer_0(RTCC_INTERNAL | RTCC_DIV_16); 
    //set_timer0(131);
	enable_interrupts(GLOBAL | INT_TIMER0 | INT_RB);
    set_timer0(131);	
	output_c(DEZENA[0] | UNIDADE[0]);
	output_d(UNIDADE[0]);
	dType = ALL;
}

#int_timer0
void trata_timer0() 
{ 
    set_timer0(131 + get_timer0()); 
	//vtimer2++;
	//if (vtimer2 == 125)
	//{
	//	secondsIn++;
	//	secondsOut++;
	//	vtimer2 = 0;
	//}
} 

#int_rb
void trata_rb()
{
	int x;
	//representa o Laço Indutivo 01
	//zera os contadores para iniciar uma nova contagem de tempo 
	if (input(CHANGE) == 0)
	{		
		switch(dType)
		{
			case IN_C:
				dType = OUT_C;
				break;
			case OUT_C:
				dType = ALL;
				break;
			case ALL:
				dType = Input;
				break;
		}
	}

	if (input(IN1) == 0)
	{	
		if (ActiveIn == 0)
			InOpen();
	}
	if (input(IN2) == 0)
	{		
		if (ActiveIn == 1)
		{
			InClose();
			Total++;
		}		
	}
	
	if (input(OUT1) == 0)
	{	
		if (ActiveOut == 0)		
			OutOpen();
	}
	if (input(OUT2) == 0)
	{		
		if (ActiveOut == 1)
			OutClose();	
	}
	
	x = input_b();
}

void ShowValueOnDisplay()
{
	long int local = 0;
	int cen = 0;
	int dec = 0;
	int unt = 0;	
	switch(dType)
	{
		case IN_C:
			local = Input;
			break;
		case OUT_C:
			local = Output;
			break;
		case ALL:
			local = Total;
			break;
	}
	if (local >= 999)
	{
		cen = 9;
		dec = 9;
		unt = 9;
	}
	else
	{
		cen = local / 100;
		dec = local / 10;
		//simula o MOD
		unt = local - (dec * 10);		
	}
	output_c(DEZENA[dec] | UNIDADE[unt]);
	output_d(UNIDADE[cen]);
}

void start()
{
	while(TRUE)
	{
		//Mostrando os valores nos displays
		ShowValueOnDisplay();
	}
}

void InOpen()
{
	output_high(CANCELA_IN);
	ActiveIn = 1;	
	Input++;
//	secondsIn = 0; 
}
void InClose()
{
	output_high(CANCELA_IN);
	ActiveIn = 0;
}

void OutOpen()
{
	output_high(CANCELA_OUT);
	ActiveOut = 1;
	Output++;
//	secondsOut = 0;
}
void OutClose()
{
	output_high(CANCELA_OUT);
	ActiveOut = 0;
}

void main()
{
	initState();
	start();
}
