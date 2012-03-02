#include <16F874.h>
#include "DigitalClock.h"
#use delay (clock=1000000)
#fuses XT,NOWDT,NOPUT
int vtimer = 0, seconds = 0;

// piscara os leds caso true
int fireAlarmState = 0;

int getClockMin()
{
	return (DEZENA[timeMinDec] | UNIDADE[timeMinUnit]);
}

int getClockHour()
{
	return DEZENA[timeHourDec] | UNIDADE[timeHourUnit];
}

int getAlarmMin()
{
	return DEZENA[alarmMinDec] | UNIDADE[alarmMinUnit];
}

int getAlarmHour()
{
	return DEZENA[alarmHourDec] | UNIDADE[alarmHourUnit];
}

void initState()
{
	timeMinUnit = 0;
	timeMinDec = 0;
	timeHourUnit = 0;	
	timeHourDec = 0;

	alarmMinUnit = 0;
	alarmMinDec = 0;
	alarmHourUnit = 0;
	alarmHourDec = 0;
	alarmActived = 0;

	state = Normal;
	fireAlarmState = 0;

	//definicoes do PIC
    setup_timer_0(RTCC_INTERNAL | RTCC_DIV_16); 
    set_timer0(131);
	enable_interrupts(GLOBAL | INT_TIMER0 | INT_RB);	
}

void minuteElapsed()
{
	if (addMinute(1))
		addHour(1);
	// checa se o alarme está ativo e o dispara
	if (alarmActived)
		checkAlarm();
}

int addMinute(int value)
{
	if (value > 0)
	{
		if (timeMinUnit < 9)
		{
			timeMinUnit += value;
			return 0;
		}
		else if (timeMinDec < 5)
		{
			timeMinDec++;
			timeMinUnit = 0;
			return 0;
		}
		else if (timeHourUnit < 9)
		{
			timeMinUnit = 0;
			timeMinDec = 0;
			return 1;
		}
	}
	else if (value < 0)
	{
		if (timeMinUnit > 0)
		{
			timeMinUnit += value;
			return 0;
		}
		else if (timeMinDec > 0)
		{
			timeMinDec--;
			timeMinUnit = 9;
			return 0;
		}
		else
		{
			timeMinDec = 5;
			timeMinUnit = 9;
			return 0;
		}
	}
}

int addHour(int value)
{
	if (value > 0)
	{
		if ((timeHourDec < 2 && timeHourUnit < 9) || (timeHourDec < 3 && timeHourUnit < 3))
		{
			timeHourUnit += value;
			return 0;
		}
		else if (timeHourDec < 2 || (timeHourDec < 3 && timeHourUnit < 3))
		{			
			timeHourUnit = 0;	
			timeHourDec++;
			return 0;
		}
		else 
		{
			timeHourUnit = 0;	
			timeHourDec = 0;
			return 1;			
		}
	}
	else if (value < 0)
	{
		if (timeHourUnit > 0)
		{
			timeHourUnit += value;
			return 0;
		}
		else if (timeHourDec > 0)
		{			
			timeHourUnit = 9;	
			timeHourDec--;
			return 0;
		}
		else 
		{
			timeHourUnit = 3;	
			timeHourDec = 2;
			return 0;			
		}
	}
}

int addMinuteAlarm(int value)
{
	if (value > 0)
	{
		if (alarmMinUnit < 9)
		{
			alarmMinUnit += value;
			return 0;
		}
		else if (alarmMinDec < 5)
		{
			alarmMinDec++;
			alarmMinUnit = 0;
			return 0;
		}
		else if (alarmHourUnit < 9)
		{
			alarmMinUnit = 0;
			alarmMinDec = 0;
			return 1;
		}
	}
	else if (value < 0)
	{
		if (alarmMinUnit > 0)
		{
			alarmMinUnit += value;
			return 0;
		}
		else if (alarmMinDec > 0)
		{
			alarmMinDec--;
			alarmMinUnit = 9;
			return 0;
		}
		else
		{
			alarmMinDec = 5;
			alarmMinUnit = 9;
			return 0;
		}
	}
}

int addHourAlarm(int value)
{
	if (value > 0)
	{
		if ((alarmHourDec < 2 && alarmHourUnit < 9) || (alarmHourDec < 3 && alarmHourUnit < 3))
		{
			alarmHourUnit += value;
			return 0;
		}
		else if (alarmHourDec < 2 || (alarmHourDec < 3 && alarmHourUnit < 3))
		{			
			alarmHourUnit = 0;	
			alarmHourDec++;
			return 0;
		}
		else 
		{
			alarmHourUnit = 0;	
			alarmHourDec = 0;
			return 1;			
		}
	}
	else if (value < 0)
	{
		if (alarmHourUnit > 0)
		{
			alarmHourUnit += value;
			return 0;
		}
		else if (alarmHourDec > 0)
		{			
			alarmHourUnit = 9;	
			alarmHourDec--;
			return 0;
		}
		else 
		{
			alarmHourUnit = 3;	
			alarmHourDec = 2;
			return 0;			
		}
	}
}
void checkAlarm()
{
	fireAlarmState = 0;
	// checa se esta na hora do alarm.
	if (getAlarmHour() == getClockHour())
		return;
	
	if (getAlarmMin() == getClockMin())
		return;
	// habilitando o alarme
	fireAlarmState = 1;
}

void fireAlarm()
{
	int t1;
	while(fireAlarmState)
	{   
		t1 = seconds;
		output_high(PIN_B2);
	    do{
			
	    }while(seconds - t1 <= 5);	

		output_low(PIN_B2);
	}	
}

void changeMode()
{
	switch(state)
	{
		case Normal:
			state = ModeTimeMin;
			break;
		case ModeTimeMin:
			state = ModeTimeHour;
			break;
		case ModeTimeHour:
			state = ModeAlarmMin;
			break;
		case ModeAlarmMin:
			state = ModeAlarmHour;
			break;
		case ModeAlarmHour:
			state = ModeAlarmActived;
			break;
		case ModeAlarmActived:
			state = Normal;
			break;
	}
}

void incClick()
{
	switch (state)
	{
		case ModeTimeMin:
			addMinute(1);
			break;
		case ModeTimeHour:
			addHour(1);
			break;
		case ModeAlarmMin:
			addMinuteAlarm(1);
			break;
		case ModeAlarmHour:
			addHourAlarm(1);
			break;
		case ModeAlarmActived:
			alarmActived = !alarmActived;
			break;
	}
}

void decClick()
{
	switch (state)
	{
		case ModeTimeMin:
			addMinute(-1);
			break;
		case ModeTimeHour:
			addHour(-1);
			break;
		case ModeAlarmMin:
			addMinuteAlarm(-1);
			break;
		case ModeAlarmHour:
			addHourAlarm(-1);
			break;
		case ModeAlarmActived:
			alarmActived = !alarmActived;
			break;
	}
}

void blinkLeds()
{
	if (alarmActived)
	{
		output_high(PIN_B1);
	}
	else
	{
		output_low(PIN_B1);
	}
	if (state == Normal)
	{
		output_low(PIN_B2);
		output_low(PIN_B3);
	}
	else if (state == ModeAlarmMin || state == ModeAlarmHour)
	{
		output_high(PIN_B3);
		output_low(PIN_B2);
	}
	else
	{
		output_high(PIN_B2);
		output_low(PIN_B3);
	}
	if (state != ModeAlarmMin && state != ModeAlarmHour)
	{
		output_c(getClockHour());
		output_d(getClockMin());
	}
	else
	{ 
		output_c(getAlarmHour());
		output_d(getAlarmMin());
	}
}

#int_timer0
void trata_timer0() 
{ 
    set_timer0(131 + get_timer0());
    vtimer++; 
    if (vtimer == 10)
	{
       vtimer = 0; 
       seconds++;
	   if (seconds > 59)
	   {
			fireAlarmState = 0;
			output_low(PIN_B1);
			seconds = 0;
			minuteElapsed();
	   }
    }
} 

#int_rb
void trata_rb()
{
	int x;	
	if (input(MODE) == 0)
	{
		changeMode(); 	
	}
	if (input(INC) == 0)
	{
		incClick(); 	
	}
	if (input(DEC) == 0)
	{
		decClick();	
	}
	x = input_b();
}

void start()
{
	while(TRUE)
	{
		blinkLeds();
		if (fireAlarmState)
			fireAlarm();
	}
}
void main()
{
	initState();
	start();
}