#define START_SB PIN_B5
#define FINISH_SB PIN_B6
#define FLASH_PHOTO PIN_D0

const int MAX_LIMIT_SPEED = 60;

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
  Traz a velocidade calculada atrav�s do valor em milisegundos
*/
float getSpeed(long int ms);

/*
  Mostra no display a velocidade em Km/h
*/
void showSpeed();
