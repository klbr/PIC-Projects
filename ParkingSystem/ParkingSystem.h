#define IN1 PIN_B4
#define IN2 PIN_B5
#define OUT1 PIN_B6
#define OUT2 PIN_B7

#define CHANGE PIN_A2
#define CANCELA_IN PIN_A3
#define CANCELA_OUT PIN_A4

enum DisplayType { IN_C, OUT_C, ALL };

DisplayType dType;

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
  Exemplo: 12 == CENTENA[0] | DEZENA[1] | UNIDADE[2];
*/

/*
  Inicia os valores da lombada eletronica e inicia o timer/int_b;
*/
void initState();

/*
  Funções de Retirar e Inserir Ticket
*/
int GetTicket();
int SetTicket();

/*
  Abre e fecha a cancela a entrada ou na saída de veículo.
*/
void InOpen();
void InClose();

void OutOpen();
void OutClose();
/*
  Mostra no display a velocidade em Km/h
*/
void ShowValueOnDisplay();
