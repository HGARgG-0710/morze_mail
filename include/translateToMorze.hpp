#ifndef TRANSLATE_TO_MORZE
#define TRANSLATE_TO_MORZE

#include <Arduino.h>

class TranslateToMorze
{
private:
    char letter;
    int _dotDelay;
    int _freq;
    
    void dot();
    void dash();
    void playEndOfTheLetter();
    void playLetter(char letter);

public:
    TranslateToMorze(int frequency,
                     int dotDelay);

    void playAllMessages(char * message1, char* message2);//main function of library for playing of message
};

#endif