#include <16F874.h>
#include "SmartSemaphore.h"
#use delay (clock=1000000)
#fuses XT,NOWDT,NOPUT
/************************************************/ 

long int countA = 0; //conta os décimos de segundo do semaforo A
long int countB = 0; //conta os décimos de segundodo semaforo B
int localSpeedA = 0; //armazena a velocidade em km/h do semaforo A
int localSpeedB = 0; //armazena a velocidade em km/h do semaforo B

int altern = 0; //alternador usado para correção na contagem dos décimos de segundos

int vtimer = 0; //timer do tempo para calculo da velocidade
int vtimer2 = 0; //timer usado incrementar os segundos
int seconds = 0; //calcula os segundos

int blinkGreenA = FALSE;
int blinkGreenB = FALSE;

long int CarsA = 0;
long int CarsB = 0;

int timeGreenA = 15;
int timeGreenB = 15;

int turnOff_FlashA = 0;
int turnOff_FlashB = 0;

int onGreen = 0;
int onYellow = 0;
int onRed = 0;

int alt_sec = FALSE;

void initState()
{
	//definicoes do PIC
    setup_timer_0(RTCC_INTERNAL | RTCC_DIV_16); 
    //set_timer0(131);
	enable_interrupts(GLOBAL | INT_TIMER0 | INT_RB);
    set_timer0(131);	
	output_c(DEZENA[0] | UNIDADE[0]);
	semaphore = Sem_A;
	output_high(GREEN_A);
	output_high(RED_B);
	output_low(YELLOW_A);
	output_low(YELLOW_B);
	output_low(RED_A);
	output_low(GREEN_B);
	onGreen = timeGreenA;
	onYellow = YELLOW_TIME;
	onRed = 1;
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

void blinkLeds()
{
	if (alt_sec == FALSE)
		output_low(SEGUNDOS);
	else
		output_high(SEGUNDOS);
	
	if (turnOff_FlashA > 0)
	{
		if (turnOff_FlashA == 3)
		{
			output_low(FLASH_A);
			if (turnOff_FlashB == 0)
				output_c(DEZENA[0] | UNIDADE[0]);
			turnOff_FlashA = 0;
		}
		turnOff_FlashA++;
	}
	
	if (turnOff_FlashB > 0)
	{
		if (turnOff_FlashB == 3)
		{
			output_low(FLASH_B);
			if (turnOff_FlashA == 0)
				output_c(DEZENA[0] | UNIDADE[0]);
			turnOff_FlashB = 0;
		}
		turnOff_FlashB++;
	}
}

#int_timer0
void trata_timer0() 
{ 
    set_timer0(131 + get_timer0());    
	vtimer++;
	vtimer2++;
    if (vtimer >= 12 && altern == 0)
	{
		countA++;
		countB++;
		vtimer = 0;
		altern = 1;
    }
	else if (vtimer >= 13)
	{
		countA++;
		countB++;
		vtimer = 0;
		altern = 0;
	}
	if (vtimer2 == 125)
	{
		seconds++;
		
		if (seconds == 15)	
		{
			calculateGreenTime();			
		}
		
		vtimer2 = 0;
		if (alt_sec == FALSE)
			alt_sec = TRUE;
		else
			alt_sec = FALSE;
			
		if (onGreen > 0)
		{			
			onGreen--;
		}
		else if (onYellow == YELLOW_TIME)
		{
			if (semaphore == Sem_A)
			{
				output_high(YELLOW_A);
				output_low(GREEN_A);
			}
			else
			{
				output_high(YELLOW_B);
				output_low(GREEN_B);
			}
			blinkGreenA = FALSE;
			onYellow--;
		}
		else if (onYellow > 0)
		{
			onYellow--;
		}
		else if (onRed > 0)
		{
			output_high(RED_A);
			output_low(YELLOW_A);
			output_high(RED_B);
			output_low(YELLOW_B);
			onRed--;
		}
		else
		{
			changeSemaphore();
			seconds = 0;
		}	
		blinkLeds();
	}
} 

void changeSemaphore()
{
	blinkGreenA = FALSE;
	blinkGreenB = FALSE;
	if (semaphore == Sem_A)
	{
		output_low(RED_B);
		output_high(GREEN_B);
		onGreen = timeGreenB;		
		semaphore = Sem_B;
		CarsB = 0;
	}
	else
	{
		output_low(RED_A);
		output_high(GREEN_A);
		onGreen = timeGreenA;		
		semaphore = Sem_A;
		CarsA = 0;
	}
	onYellow = YELLOW_TIME;
	onRed = 1;
}

#int_rb
void trata_rb()
{
	int x;
	//representa o Laço Indutivo 01 que conta a passagem dos carros e inicia o timer para
	//calculo da velocidade
	//zera os contadores para iniciar uma nova contagem de tempo 
	if (input(BTN_A1) == 0)
	{		
		countA = 0;		
		localSpeedA = 0;		
		if (semaphore == Sem_A)
			CarsA++;
		if (semaphore == Sem_B)
		{
			output_high(FLASH_A);
			turnOff_FlashA = 1;
		}
	}
	
	if (input(BTN_B1) == 0)
	{		
		countB = 0;		
		localSpeedB = 0;		
		if (semaphore == Sem_B)
			CarsB++;
		if (semaphore == Sem_A)
		{
			output_high(FLASH_B);
			turnOff_FlashB = 1;
		}
	}
	//representa o Laço Indutivo 02
	//Exibe a velocidade e dispara a foto caso a mesma seja maior que o máximo permitido
	if (input(BTN_A2) == 0)
	{		
		localSpeedA = (int)getSpeed(countA);
		if (localSpeedA > MAX_LIMIT_SPEED)
		{
			output_high(FLASH_A);			
		}
		turnOff_FlashA = 1;
	}
	
	if (input(BTN_B2) == 0)
	{		
		localSpeedB = (int)getSpeed(countB);
		if (localSpeedB > MAX_LIMIT_SPEED)
		{
			output_high(FLASH_B);			
		}
		turnOff_FlashB = 1;
	}
	x = input_b();
}

void showSpeed(int km_h)
{
	int dec = 0;
	int unt = 0;				
	if (km_h >= 99)
	{
		dec = 9;
		unt = 9;
	}
	else
	{
		dec = km_h / 10;
		//simula o MOD
		unt = km_h - (dec * 10);		
	}
	output_c(DEZENA[dec] | UNIDADE[unt]);
	//zera a velocidade local
	km_h = 0;
}

void calculateGreenTime()
{
	timeGreenA = 15;
	if (CarsA > CarsB + ((CarsB * 6)/10))
	{
		timeGreenA = 40;
		return;
	}
	
	if ((CarsA > CarsB + ((CarsB * 3)/10)) && (CarsA <= CarsB + ((CarsB * 6)/10)))
	{
		timeGreenA = 35;
		return;
	}
	
	if (CarsA >= CarsB +((CarsB * 3)/10))
	{
		timeGreenA = 30;
		return;
	}
}

void start()
{
	while(TRUE)
	{	
		if (localSpeedA > 0)
		{
			showSpeed(localSpeedA);
			localSpeedA = 0;
		}
		if (localSpeedB > 0)
		{
			showSpeed(localSpeedB);	
			localSpeedB = 0;
		}
	}
}

void main()
{
	initState();
	start();
}
