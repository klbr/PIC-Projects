#include <16F874.h>
#include "ParkingSystem.h"
#use delay (clock=1000000)
#fuses XT,NOWDT,NOPUT
/************************************************/ 

//registra as quantidades que entraram e saíram
long int Input = 99;
long int Output = 0;

//controlador de status da cancelas e sensores
int ActiveIn = 0;
int ActiveOut = 0;

void initState()
{
	//definicoes do PIC
    setup_timer_0(RTCC_INTERNAL | RTCC_DIV_16); 
	enable_interrupts(GLOBAL | INT_TIMER0 | INT_RB | INT_EXT);
    set_timer0(131);
	//Zerando os displays
	output_c(DEZENA[0] | UNIDADE[0]);
	output_d(DEZENA[0] | UNIDADE[0]);
	//Mostra o total estacionado
	dType = ALL;
}

#int_timer0
void trata_timer0() 
{ 
    set_timer0(131 + get_timer0()); 
	//Tive que tratar o input_a e output_a aqui

	//ativando o sensor entrada 1, caso não esteja lotado
	if (input(INPUT_A) == 0)
	{
		if (MAX_VAGAS <= Input - Output)
			return;
		if (ActiveIn != 2)
			ActiveIn = 1;
	}
	
	//ativando o sensor de saida 1, caso exista carros estacionados
	//senao ficaria negativo
	if (input(OUTPUT_A) == 0)
	{
		if (Input - Output <= 0)
			return;
		if (ActiveOut != 2)
			ActiveOut = 1;
	}
} 

#int_ext
void trata_ext()
{
	//Motorista recebe o ticket e a cancela se abre,
	if (input(GET_TICKET) == 0)
	{
		// caso esteja lotado, sai, não abre a cancela e nem da o ticket
		if (MAX_VAGAS <= Input - Output)
			return;
		if (ActiveIn == 1)
			InOpen();
	}
}

#int_rb
void trata_rb()
{
	int x;

	//muda a exbibição dos displays
	if (input(CHANGE) == 0)
	{		
		switch(dType)
		{
			case IN_C:
				//Apenas as saidas
				dType = OUT_C;
				break;
			case OUT_C:
				//total estacionado
				dType = ALL;
				break;
			case ALL:
				//total que entrou
				dType = IN_C;
				break;
		}
	}
	
	//fecha a cancela depois q o motorista passa pelo sensor 2
	if (input(INPUT_CAN) == 0)
	{		
		if (ActiveIn == 2)
		{
			InClose();
		}		
	}
	
	//ao colocar o ticket a cancela abre
	if (input(PUT_TICKET) == 0)
	{
		if (Input - Output <= 0)
			return;
		if (ActiveOut == 1)
			OutOpen();
	}
	//depois que o carro sai, a cancela fecha
	if (input(OUTPUT_CAN) == 0)
	{		
		if (ActiveOut == 2)
			OutClose();	
	}
	x = input_b();
}

void ShowValueOnDisplay()
{
	long int local = 0;
	long int cen = 0;
	long int dec = 0;
	long int unt = 0;	
	switch(dType)
	{
		case IN_C:
			local = Input;
			break;
		case OUT_C:
			local = Output;
			break;
		case ALL:
			local = Input - Output;
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
		unt = local - (dec * 10) - (cen * 100); 
	}
	output_c(DEZENA[dec] | UNIDADE[unt]);
	output_d(DEZENA[0] | UNIDADE[cen]);
}

void InOpen()
{
	output_high(CANCELA_IN);
	ActiveIn = 2;	
}
//ao fechar a cancela da entrar o valor é incrementado
void InClose()
{
	output_low(CANCELA_IN);
	ActiveIn = 0;
	Input++;
	//lotado é ativado depois q o carro entra
	//e se esse carro for o máximo permitido
	if (MAX_VAGAS <= Input - Output)	
		output_high(LOTADO);
}

void OutOpen()
{
	output_high(CANCELA_OUT);
	ActiveOut = 2;	
}
//ao fechar a cancela de saida o valor é incrementado
void OutClose()
{
	//ao sair pelo menos um carro, o lotado deve apagar
	output_low(CANCELA_OUT);
	output_low(LOTADO);
	ActiveOut = 0;
	Output++;
}

void start()
{
	while(TRUE)
	{
		ShowValueOnDisplay();
	}
}

void main()
{
	initState();
	start();
}
