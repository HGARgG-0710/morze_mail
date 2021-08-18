#ifndef TRANSLATE_TO_MORZE
#define TRANSLATE_TO_MORZE

#include <Arduino.h>

class MorzeTranslator
{
private:
    char letter;
    int _dotDelay;
    int _freq;

    void dot();
    void dash();
    void playEndOfTheLetter();
    void playLetter(char);

public:
    MorzeTranslator(int,
                     int);

    void playAllMessages(char *, char *); // main function of library for playing the message
};

#endif