#define MODE PIN_B6
#define INC PIN_B5
#define DEC PIN_B4

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

enum ClockState
{
	//Estado de Normal. Inicial e não muda valores do relógio digital
	Normal, 

	//Modos do relógio de Minutos e Horas
	ModeTimeMin, 
	ModeTimeHour, 
	
	//Modos do Alarme de Minutos e Horas
	ModeAlarmMin, 
	ModeAlarmHour,

	//modos do alarme: ativo ou nao
	ModeAlarmActived
};


	//variáveis do relógio digital em suas unidades e dezenas de acordo com minutos e horas
	int timeMinUnit;
	int timeMinDec;
	int timeHourUnit;
	int timeHourDec;

	//variáveis do alarme do relógio digital em suas unidades e dezenas de acordo com minutos e horas
	int alarmMinUnit;
	int alarmMinDec;
	int alarmHourUnit;
	int alarmHourDec;
	
	//valor do estado de máquina do relógio- seleciona os modos do relógio
	ClockState state;
	
	//Se o alarme esta ativo o nao
	int alarmActived;


/*
  Exemplo: 12 == DEZENA[1] | UNIDADE[2];
*/

/*
  Inicia os valores do clock e inicia o timer para contar os minutos;
*/
void initState();

/*
Chamado quando um minuto se concretiza
*/
void minuteElapsed();

/*
Verifica se o status do alarme. Para disparar caso o tempo de alarme esteja igual a hora atual
*/
void checkAlarm();
/*
Retorna os minutos do relógio
*/
int getClockMin();
/*
Retorna a hora do relógio
*/
int getClockHour();
/*
Retorna o minuto do Alarme
*/
int getAlarmMin();
/*
Retorna a hora do Alarme
*/
int getAlarmHour();
/*
Muda o modo do relógio digital 
*/
void changeMode();
/*
Incrementa ou Decrementa os minutos do relógio
*/
int addMinute(int value);
/*
Incrementa ou Decrementa as horas do relógio
*/
int addHour(int value);
/*
Incrementa ou Decrementa os minutos do alarme
*/
int addMinuteAlarm(int value);
/*
Incrementa ou Decrementa os horas do alarme
*/
int addHourAlarm(int value);
/*
Liga/Desliga os Leds do Relógio
*/
void blinkLeds();