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

//Usadas para piscar o verde quando atingir 5 segudos antes da troca pelo amarelo
int blinkGreenA = FALSE;
int blinkGreenB = FALSE;
//Variáveis de contagem dos veículos captados pelos sensores
long int CarsA = 0;
long int CarsB = 0;
//tempo de sinal verde para os semáforos
int timeGreenA = 15;
int timeGreenB = 15;
//variável que desliga o flash e o display de velocidade após 2 segundos
int turnOff_FlashA = 0;
int turnOff_FlashB = 0;
//variaveis que são usadas para contar o tempo efetivo em segundos em cada estado do semaforo
int onGreen = 0;
int onYellow = 0;
int onRed = 0;
//alternador do segundo, usado para ajudar na medição do tempo visível apenas no proteus
int alt_sec = FALSE;

void initState()
{
	//definicoes do PIC
    setup_timer_0(RTCC_INTERNAL | RTCC_DIV_16); 
    //set_timer0(131);
	enable_interrupts(GLOBAL | INT_TIMER0 | INT_RB);
    set_timer0(131);
	//iniciando as variáveis e outputs
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
	//alternado o piscar dos segundos
	if (alt_sec == FALSE)
		output_low(SEGUNDOS);
	else
		output_high(SEGUNDOS);
	
	//desligando os displays e flash ocasionado pelo sensor do semaforo A
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
	//desligando os displays e flash ocasionado pelo sensor do semaforo B
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
    //incremento do tempo usado para calculo da velocidade
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
		vtimer2 = 0;
		//realizando o calculo após 15 segundos de semaforo ligado
		if (seconds == 15)
			calculateGreenTime();			
		
		//alternador do LED dos segundos
		if (alt_sec == FALSE)
			alt_sec = TRUE;
		else
			alt_sec = FALSE;
		
		//Decrementa o tempo de onGreen até zerar e passar para o proximo estágio
		if (onGreen > 0)
		{			
			onGreen--;
		}
		//estagio amarelo
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
		//decrementando até zerar
		else if (onYellow > 0)
		{
			onYellow--;
		}
		//estado de Vermelho para os dois semaforos
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
		//fazendo a troca de semaforo para estado Verde e zerando o segundo para novo calculo
			changeSemaphore();
			seconds = 0;
		}
		//Executa o desligamento ou ligamento dos LEDs e displays
		blinkLeds();
	}
} 

void changeSemaphore()
{
	blinkGreenA = FALSE;
	blinkGreenB = FALSE;
	//Alternando de semaforo e jogando o novo valor de timeGreen para o onGreen do semaforo atual
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
	//iniciando os valores padrão do onYellow e onRed
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
		//Incrementa a contagem do carro usando apenas o botão A1
		if (semaphore == Sem_A)
			CarsA++;
		//Semáforo A vermelho, então o Flash é ativado. Avanço do sinal vermelho
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
		//Incrementa a contagem do carro usando apenas o botão B1
		if (semaphore == Sem_B)
			CarsB++;
		//Semáforo B vermelho, então o Flash é ativado. Avanço do sinal vermelho
		if (semaphore == Sem_A)
		{
			output_high(FLASH_B);
			turnOff_FlashB = 1;
		}
	}
	//representa o Laço Indutivo 02
	//Exibe a velocidade e dispara a foto caso a mesma seja maior que o máximo permitido
	//Vale tanto para o semaforo A quanto para o B
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
	//forma simplificada de mostrar o valor de display
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
	//calculo do tempo em A. Especificado no documento
	//Seria possivel alterar para preferência em B do mesmo modo que é 
	//em A
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
		//Metodo principal de mostrar a velocidade
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
	//instanciando o inicio do programa e executando o programa
	initState();
	start();
}
