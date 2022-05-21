#include "morzeTranslator.hpp"
#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 4, 3, 2); // (RS, E, DB4, DB5, DB6, DB7)

#define BUTTONS_PIN A2

const char EngLetterArray[28] = {
	'a', 'b', 'c', 'd', 'e',
	'f', 'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n', 'o',
	'p', 'q', 'r', 's', 't',
	'u', 'v', 'w', 'x', 'y',
	'z', '.', ' '};
const uint8_t NumOfButton[33] = {
	/* First row */ 48, 52, 50, 54, 49, 53, 51, 55,
	/* Second row */ 0, 4, 2, 6, 1, 5, 3, 7,
	/* Third row */ 16, 20, 18, 22, 17, 21, 19, 23,
	/* Fourth row */ 32, 36, 34,
	/* 38 - Enter (or mode change); 33, 37 - shift right or left; 35 - BackSpace*/ 39,
	/*special buttons*/ 38, 33, 37, 35, 8};

MorzeTranslator translator(4000, 250);

uint8_t stringNumber = 0; // the string index (either 1 or 0)

volatile uint8_t lastPressedButtonNumber = 0; // code of the last pressed button
volatile bool buttonWasPressed = false;
volatile int timePressed = 0; // count is made in millis

char *message1 = new char[50];
char *message2 = new char[50];
char cToStr[2];

// This function updates the LCD.
void printLCD()
{
	lcd.clear();

	if (strlen(message1) >= 8)
	{
		lcd.setCursor(0, 0);
		char *cutMessage1 = message1 + strlen(message1) - 8;
		lcd.print(cutMessage1);
	}

	if (strlen(message2) >= 8)
	{
		lcd.setCursor(0, 1);
		char *cutMessage2 = message1 + strlen(message2) - 8;
		lcd.print(cutMessage2);
	}

	if (strlen(message1) < 8)
	{
		lcd.setCursor(0, 0);
		lcd.print(message1);
	}

	if (strlen(message2) < 8)
	{
		lcd.setCursor(0, 1);
		lcd.println(message2);
	}

	Serial.print("msg1 = ");
	Serial.println(message1);
	Serial.print("msg2 = ");
	Serial.println(message2);
}

// This function adds the character to the message.
void MessagePlusFunct(size_t letter)
{
	cToStr[0] = EngLetterArray[letter];
	if (stringNumber == 0)
		message1 = strcat(message1, cToStr);
	else if (stringNumber == 1)
		message2 = strcat(message2, cToStr);
	printLCD();
}

// This function negates one last character.
void MessageMinusFunct()
{
	if (stringNumber == 0)
		message1[strlen(message1) - 2] = '\0';
	else
		message2[strlen(message2) - 2] = '\0';
	printLCD();
}

// This function handles a certain button input. 
void buttonHandle(int buttonNumber)
{
	if (analogRead(BUTTONS_PIN) == 0)
	{
		delay(20);
		if (analogRead(BUTTONS_PIN) == 0)
		{
			if (!buttonWasPressed)
			{
				buttonWasPressed = true;
				lastPressedButtonNumber = buttonNumber;
				timePressed = 0;

				Serial.println("Pressed " + String(lastPressedButtonNumber) + "button");
			}
			timePressed += 120; // ! Check, whether you should add 100, 20 or 120. 
			Serial.println("Button has been hold for " + String(timePressed) + " millis.");
		}
	}
	else
	{
		if (buttonWasPressed && buttonNumber == lastPressedButtonNumber)
		{
			buttonWasPressed = false;
			Serial.println("Unpressed " + String(lastPressedButtonNumber) + "button");

			switch (lastPressedButtonNumber)
			{
			case 38:
				stringNumber = stringNumber == 0 ? 1 : 0;
				break;

			case 33:
				lcd.scrollDisplayLeft();
				break;

			case 37:
				lcd.scrollDisplayRight();
				break;

			case 35:
				MessageMinusFunct();
				break;

			case 8:
				if (timePressed > 5000)
					translator.playAllMessages(message1, message2);
				else
				{
					// TODO: Add the mode change and russian language support (a lot of work with the ASCII table).
					Serial.println("We are ready to change mode");
				}
				break;

			default:
				// Checking for the first 28 buttons to be pressed.
				for (uint8_t i = 0; i < 28; i++)
					if (lastPressedButtonNumber == NumOfButton[i])
						MessagePlusFunct(i);
			}
		}
	}
}

void setup()
{
	cToStr[1] = '\0';
	message1 = "";
	message2 = "";

	lcd.begin(8, 2);
	lcd.setCursor(0, 0); // * IMPORTANT: First number is for symbol in string, the second one is for string index.
	Serial.begin(9600);

	pinMode(8, OUTPUT);	 // Demultiplexer CD4051 pin 11 (A)
	pinMode(9, OUTPUT);	 // Demultiplexer CD4051 pin 10 (B)
	pinMode(10, OUTPUT); // Demultiplexer CD4051 pin 9  (C)

	pinMode(7, OUTPUT); // Demultiplexer CD4051
	pinMode(6, OUTPUT); // Demultiplexer CD4051
	pinMode(5, OUTPUT); // Demultiplexer CD4051

	lcd.print("Hello,");
	lcd.setCursor(0, 1);
	lcd.print("User!");

	Serial.println("Hello, World!");
}

void loop()
{
	for (uint8_t i = 0; i < 8; i++)
	{
		// LOL, this "magic" part was written by my teacher.
		// To support the project further, will have to google what the heck it means :P
		PORTB = (PORTB & 0b11111000) ^ i;

		for (uint8_t j = 0; j < 8; j++)
		{
			// Another "What the...?" piece :P
			// Most funnily, all the stuff worked, so it should be perfectly correct. 
			PORTD = (PORTD & 0b00011111) ^ (j << 5);
			buttonHandle(i * 8 + j);
		}
	}
	delay(100);
}
