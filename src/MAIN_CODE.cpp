#include "translateToMorze.hpp"
#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 4, 3, 2); // (RS, E, DB4, DB5, DB6, DB7)

#define BUTTONS_PIN A2

char EngLetterArray[28] = {
    'a', 'b', 'c', 'd', 'e',
    'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't',
    'u', 'v', 'w', 'x', 'y',
    'z', '.', ' '};

uint8_t NumOfButton[33] = {
  /* 1-я строчка */ 48, 52, 50, 54, 49, 53, 51, 55,
  /* 2-я строчка */ 0, 4, 2, 6, 1, 5, 3, 7,
  /*3-я строчка*/ 16, 20, 18, 22, 17, 21, 19, 23,
  /*4-я строчка*/ 32, 36, 34,
  /*38 - Enter; 33, 37 - сдвиг вправо, влево; 35 - BackSpace*/ 39,
  /*special buttons*/38, 33, 37, 35, 8};

TranslateToMorze translator(4000, 250);

uint8_t stringNumber = 0; // номер строки

volatile uint8_t lastPressedButtonNumber = 0; // - код(номер) последней нажатой кнопки
volatile uint8_t buttonWasPressed = 0; // 0 - was not pressed, 1 - was pressed

volatile int timePressed = 0;//it counts in millis

char *Message1 = new char[50];
char *Message2 = new char[50];

void PrintAtLCDFunct() {
  lcd.clear();

  if (strlen(Message1) >= 8) {
    lcd.setCursor(0, 0);
    char *cutMessage1 = Message1 + strlen(Message1) - 8;
    lcd.print(cutMessage1);
  }

  if (strlen(Message2) >= 8) {
    lcd.setCursor(0, 1);
    char *cutMessage2 = Message1 + strlen(Message2) - 8;
    lcd.print(cutMessage2);
  }

  if (strlen(Message1) < 8) {
    lcd.setCursor(0, 0);
    lcd.print(Message1);
  }

  if (strlen(Message2) < 8) {
    lcd.setCursor(0, 1);
    lcd.println(Message2);
  }
  Serial.print("msg1 = ");
  Serial.println(Message1);
  Serial.print("msg2 = ");
  Serial.println(Message2);
}

char cToStr[2];

void MessagePlusFunct(size_t letter) {
  cToStr[0] = EngLetterArray[letter];
  Serial.println("message+");

  if (stringNumber == 0) {
    Message1 = strcat(Message1, cToStr);
  }
  else if(stringNumber == 1) {
    Message2 = strcat(Message2, cToStr);
  }
  PrintAtLCDFunct();
}

void MessageMinusFunct() {
  if (stringNumber == 0) {
    Message1[strlen(Message1) - 2] = '\0';
  } else {
    Message2[strlen(Message2) - 2] = '\0';
  }
  PrintAtLCDFunct();
}

void setup() {
  cToStr[1] = '\0';
  Message1 = "";
  Message2 = "";

  lcd.begin(8, 2);
  lcd.setCursor(0, 0); //*IMPORTANT: First number is for symbol in string, the second one is for string.
  Serial.begin(9600);

  pinMode(8, OUTPUT);  // Demultiplexer CD4051 pin 11 (A)
  pinMode(9, OUTPUT);  // Demultiplexer CD4051 pin 10 (B)
  pinMode(10, OUTPUT); // Demultiplexer CD4051 pin 9  (C)

  pinMode(7, OUTPUT); // Demultiplexer CD4051
  pinMode(6, OUTPUT); // Demultiplexer CD4051
  pinMode(5, OUTPUT); // Demultiplexer CD4051

  lcd.print("Hello,");
  lcd.setCursor(0, 1);
  lcd.print("User!");

  Serial.println("Hello, World!");
}

void buttonHandle(int buttonNumber) {
  if (analogRead(BUTTONS_PIN) == 0) {
    delay(20);
    if (analogRead(BUTTONS_PIN) == 0) {
      if (buttonWasPressed == 0) {
        buttonWasPressed = 1;
        lastPressedButtonNumber = buttonNumber;
        Serial.println("Pressed " + String(lastPressedButtonNumber) + "button");

        timePressed = 0;
      }
      timePressed += 20;
      Serial.println("Button has been hold for " + String(timePressed) + " millis.");
    }
  }
  else {
    if (buttonWasPressed == 1 && buttonNumber == lastPressedButtonNumber) {
      buttonWasPressed = 0;
      Serial.println("Unpressed " + String(lastPressedButtonNumber) + "button");

      for (uint8_t i = 0; i < 33; i++) {
        if (lastPressedButtonNumber == NumOfButton[i] 
        && (lastPressedButtonNumber != 33) 
        && (lastPressedButtonNumber != 35) 
        && (lastPressedButtonNumber != 37) 
        && (lastPressedButtonNumber != 38) 
        && (lastPressedButtonNumber != 8)) {
          MessagePlusFunct(i);
        }
      }

      if (lastPressedButtonNumber == 38) {
        stringNumber = stringNumber == 0 ? 1 : 0;
      }
      if (lastPressedButtonNumber == 33) {
        lcd.scrollDisplayLeft();
      }
      if (lastPressedButtonNumber == 37) {
        lcd.scrollDisplayRight();
      }
      if (lastPressedButtonNumber == 35) {
        MessageMinusFunct();
      }
      if (lastPressedButtonNumber == 8) {
        if(timePressed > 5000) {
          Serial.println("We are ready to play");
          translator.playAllMessages(Message1, Message2);
        } else {
          Serial.println("We are ready to change mode");
        }
      }
    }
  }
}

void loop() {
  for (uint8_t i = 0; i < 8; i++) {
    PORTB = (PORTB & 0b11111000) ^ i;

    for (uint8_t j = 0; j < 8; j++) {
      PORTD = (PORTD & 0b00011111) ^ (j << 5);

      buttonHandle(i * 8 + j);
    }
  }
  delay(100);
}
