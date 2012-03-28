#define FLASH_A PIN_D0
#define FLASH_B PIN_D1
#define RED_A PIN_D2
#define RED_B PIN_D3
#define YELLOW_A PIN_D4
#define YELLOW_B PIN_D5
#define GREEN_A PIN_D6
#define GREEN_B PIN_D7

#define SEGUNDOS PIN_A5

#define BTN_A1 PIN_B4
#define BTN_A2 PIN_B5
#define BTN_B1 PIN_B6
#define BTN_B2 PIN_B7

const int MAX_LIMIT_SPEED = 60;
const int YELLOW_TIME = 3;

enum StateSemaphore { Sem_A, Sem_B };

StateSemaphore semaphore;

const byte DEZENA[10] =
{
	0b00000000,
	0b00010000,
	0b00100000,
	0b00110000,
	0b01000000,
	0b01010000,
	0b01100000,
	0b01110000,
	0b10000000,
	0b10010000,
};

const byte UNIDADE[10] =
{
	0b00000000,
	0b00000001,
	0b00000010,
	0b00000011,
	0b00000100,
	0b00000101,
	0b00000110,
	0b00000111,
	0b00001000,
	0b00001001,
};

/*
  Exemplo: 12 == DEZENA[1] | UNIDADE[2];
*/

/*
  Inicia os valores da lombada eletronica e inicia o timer/int_b;
*/
void initState();

/*
  Traz a velocidade calculada através do valor em milisegundos
*/
float getSpeed(long int ms);

/*
  Mostra no display a velocidade em Km/h
*/
void showSpeed(int km_h);

/*
  Pisca os leds pre-selecionados para piscar
*/
void blinkLeds();

/*
  Calcula o tempo do semaforo verde de acordo com o percentual do fluxo entre as vias A e B
*/
void calculateGreenTime();

/*
  Troca o semaforo que esta em verde para o outro
*/
void changeSemaphore();