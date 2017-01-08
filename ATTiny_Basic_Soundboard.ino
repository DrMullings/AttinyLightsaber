/*
 Name:		ATTiny_Basic_Soundboard.ino
 Created:	03.06.2016 17:31:42
 Author:	DrMullings
*/

#include <avr/sleep.h>
#include <avr/power.h>

/*******************************************************************************
* Pin definitions
*******************************************************************************/

#define SWING_PIN 3		//physical 2
#define CLASH_PIN 4		//physical 3
#define BLADE_PIN 0		//physical 5
#define WT_PIN 1		//physical 6
#define BUTTON_PIN 2	//physical 7

/*******************************************************************************
* WT Command definitions
*******************************************************************************/

#define WT588D_LOOP_START		0xF2
#define WT588D_LOOP_STOP		0xFE
//#define WT588D_IOEXT_ON         0xF5
//#define WT588D_IOEXT_OFF        0xF6
#define WT588D_VOL_MIN		    0xE0
#define WT588D_VOL_MAX		    0xE7

#define SABER_ON 	0
#define SABER_OFF 	1
#define HUM_IDLE 	2
#define SWING_0		3
#define SWING_1 	4
#define SWING_2		5
#define SWING_3		6
#define SWING_4		7
#define SWING_5		8
#define SWING_6		9
#define SWING_7		10
#define CLASH_0		11
#define CLASH_1		12
#define CLASH_2		13

boolean saberIsOn = false;
boolean buttonPressed = false;

int swingArray[8][8] = { {3,507},{4,641},{5,983},{6,2028},{7,542},{8,580},{9,1230},{10,539} };
int clashArray[3][3] = { {11,690},{12,542},{13,580} };

// the setup function runs once when you press reset or power the board
void setup() {
	
	pinMode(SWING_PIN, INPUT_PULLUP);
	pinMode(CLASH_PIN, INPUT_PULLUP);
	pinMode(BUTTON_PIN, INPUT_PULLUP);

	digitalWrite(BUTTON_PIN, HIGH);

	pinMode(WT_PIN, OUTPUT);

	pinMode(BLADE_PIN, OUTPUT);
	digitalWrite(BLADE_PIN, LOW);

	saberIsOn = false;

	WT_Send_Command(WT588D_VOL_MAX);

	attachInterrupt(0, do_button, RISING);

	//set_sleep_mode(SLEEP_MODE_PWR_SAVE);  //festlegen des Schlafmoduses
	//sleep_enable();  //ermoeglichen der angegebenen Schlaffunktion

}

// the loop function runs over and over again until power down or reset
void loop() {

	if (buttonPressed)
	{
		buttonPressed = false;

		if (saberIsOn)
		{
			saberIsOn = false;
			//turn off
			WT_Send_Command(SABER_OFF);
			delay(100);
			for (int i = 255; i >= 0; i--)
			{
				analogWrite(BLADE_PIN, i);
			}
			//sleep_mode();
		}
		else
		{
			saberIsOn = true;
			//turn on
			//sleep_disable();
			//power_all_enable();
			WT_Send_Command(SABER_ON);
			delay(100);
			for (int i = 0; i < 255; i++)
			{
				analogWrite(BLADE_PIN, i);
			}
			delay(1600);
			WT_Send_Command(HUM_IDLE);
			delay(100);
			WT_Send_Command(WT588D_LOOP_START);
			delay(100);
		}
		
		attachInterrupt(0, do_button, RISING);
	}

	if (!saberIsOn)
	{
		//sleep sweet summer child
		//sleep_mode();
	}
	else
	{
		if (digitalRead(BUTTON_PIN) == LOW)
		{
			do_button();
			delay(100);
		}

		if (digitalRead(CLASH_PIN) == LOW)
		{
			long playNmb = millis() % 3;
			WT_Send_Command(clashArray[playNmb][0]);
			delay(clashArray[playNmb][1]);

			WT_Send_Command(HUM_IDLE);
			delay(100);
			WT_Send_Command(WT588D_LOOP_START);
		}

		if (digitalRead(SWING_PIN) == LOW)
		{
			long playNmb = millis() % 8;
			WT_Send_Command(swingArray[playNmb][0]);
			delay(swingArray[playNmb][1]);

			WT_Send_Command(HUM_IDLE);
			delay(100);
			WT_Send_Command(WT588D_LOOP_START);
		}


	}
}

void do_button()
{
	buttonPressed = true;
	detachInterrupt(0);
}


void WT_Send_Command(byte addr) {
	digitalWrite(WT_PIN, LOW);
	delay(5);

	for (int i = 0; i < 8; i++) {
		digitalWrite(WT_PIN, HIGH);
		if (bitRead(addr, i)) {
			delayMicroseconds(600);
			digitalWrite(WT_PIN, LOW);
			delayMicroseconds(200);
		}
		else {
			delayMicroseconds(200);
			digitalWrite(WT_PIN, LOW);
			delayMicroseconds(600);
		}
	}

	digitalWrite(WT_PIN, HIGH);
	delay(10);
}