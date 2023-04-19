// ? Perhaps, write some kind of text-generator tool with which to write a generalization for this thing?
#include "MorzeTranslator.hpp"
#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 4, 3, 2); // (RS, E, DB4, DB5, DB6, DB7)

#define BUTTONS_PIN A2

// TODO: extend the number of characters and also -- the number of buttons: (check for adding cyrillyc) -- 28 + 33 = 61; One would still want to make use of the other characters from the table in morzeTranslator.cpp;
// TODO: rename the files of the project; Let they more simply and straightforwardly represent what they're for there...
// TODO: along with doing the previous one, pray do understand how does the PlatrformIO work (semi-again); Plus, install it properly (not just as a VSCODE extension...)
// TODO: one may want to use a more powerful arduino board for the next prototype of this thing.
// TODO: one may also want to build one's own board and have the thing being one single PCB;
// TODO: one may also want to have some sort of 3d model for some wrapper-carcass for the printed board;
// TODO: along with such a scheme, one would (probably) want to generalize it as well -- for there to be generators of:
// * 1. Source code;
// * 2. Corresponding pcb scheme + all other pcb-related stuff;
// * 3. Corresponding 3d model;
// * 4. Corresponding Arduino-prototype scheme (if it is feasible for it to be...); 
// * All this'd be morze_mail 3.0;
// ! Notice: if one was to allow inclusion of arbitrary character sets to the thing, one would also require for the pcb in question to have corresponding amount of memory...
// ? Question: perhaps, one would also extend the thing not on a purely technical (how it's done or how big it can potentially be), but also the architectural level (what can it conceptually do);
// * For instance, what about, long-distance version? (For testing of this on Arduino, one may want to pick a certain detail [for instance, RS-485])

// ^ Now, that's just not serious! XD
// ^ Each and every time one happens to start/continue to work on something and make thousands of beautiful creative notes,
// ^ one also ends up flooding the project with them!
// TODO: create a new format for TODOS and Notes (call it that even: 'Tunnel' -- from TANL [Todos And Notes Language]); Create syntax highlighter and formatter for it...

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

// TODO: this should be generalizable to an arbitrary 'n' rows for an arbitrary display...
char *message[2] = {new char[50], new char[50]};
char cToStr[2];

// This function updates the LCD.
void printLCD()
{
	lcd.clear();

	// ! This code is repeating;
	// TODO: make a single static 'message[2][50]' array instead of two dynamically allocated and run loops through it, to avoid doing this;
	// TODO: same goes for the 'morzeTranslator' thing...
	// TODO: try and revisit the project again -- try running it even.
	// * And understand that thing with ports finally already....
	for (int i = 0; i < 2; i++)
	{
		lcd.setCursor(0, i);
		if (strlen(message[i]) >= 8)
		{
			char *cutMessage = message[i] + strlen(message[i]) - 8;
			lcd.print(cutMessage);
		}
		else
		{
			// ? Why was it there, actually?
			// * Pray find out...
			if (i == 0)
				lcd.print(message[i]);
			else
				lcd.println(message[i]);
		}

		char number[2];
		sprintf(number, "%d", i);
		char concat[8];
		strcpy(concat, "msg");
		strcat(concat, number);
		strcat(concat, " = ");
		Serial.print(concat);
		Serial.println(message[i]);
	}
}

// This function adds the character to the message.
void MessagePlusFunct(size_t letter)
{
	cToStr[0] = EngLetterArray[letter];
	message[stringNumber] = strcat(message[stringNumber], cToStr);
	printLCD();
}

// This function negates one last character.
void MessageMinusFunct()
{
	message[stringNumber][strlen(message[stringNumber]) - 2] = '\0';
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
			timePressed += 20;
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
				// ? Why whole 5 seconds?
				// TODO: try lesser amount of time, how that'd do...;
				// * There is 1 button for 2 actions, separated by different pressing timespan;
				// ! I don't like it/
				// TODO: add another button to it...
				// TODO: create the new scheme for this thing...
				if (timePressed > 5000)
					translator.playAllMessages(message[0], message[1]);
				else
				{
					// TODO: Add the mode change and russian language support (a lot of work with the ASCII table).
					// * Actually, one should check -- Arduino Uno may not have enough memory for this sort of stuf....
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
	message[0] = "";
	message[1] = "";

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
		// * Starting to fathom it a little bit better now, but still not entirely;
		PORTB = (PORTB & 0b11111000) ^ i;

		for (uint8_t j = 0; j < 8; j++)
		{
			// Another "What the...?" piece :P
			// Most funnily, all the stuff worked, so it should be perfectly correct.
			PORTD = (PORTD & 0b00011111) ^ (j << 5);
			buttonHandle(i * 8 + j);
		}
	}
	// ? Why not make it smaller or larger? 
	// * Pray do try... Will it affect the user experience? 
	delay(100);
}
