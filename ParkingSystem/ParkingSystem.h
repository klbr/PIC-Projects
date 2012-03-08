//Portas dos sensores e do ticket da cancela de entrada
#define INPUT_A PIN_B1
#define INPUT_CAN PIN_B4
#define GET_TICKET PIN_B3

//Portas dos sensores e do ticket da cancela de saída
#define OUTPUT_A PIN_B2
#define OUTPUT_CAN PIN_B7
#define PUT_TICKET PIN_B6

//Porta para a mudança dos Valores dos display das quantidades
//Entrada - Saida - Estacionado
#define CHANGE PIN_B5

//Leds que indicam a subida da cancela 
#define CANCELA_IN PIN_A0
#define CANCELA_OUT PIN_A1

//Led que indica que está lotado o estacionamento
#define LOTADO PIN_A2

//Quantidade máxima de vagas
long int MAX_VAGAS = 100;

//Estados dos displays que é alterado pelo click do CHANGE 
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
  Exemplo: 112 == CENTENA[1] | DEZENA[1] | UNIDADE[2];
*/

/*
  Inicia os valores do sistema e inicia o timer/int_b/int_ext;
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
  Mostra no display a quantidade selecionada dos carros
*/
void ShowValueOnDisplay();
