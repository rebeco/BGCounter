/*
 sevenSegment.h - Library for control a seven segment digit display.
 Created by Pedro León, March 22, 2015.
 Released into the public domain.
 */
#ifndef sevenSegment4d12p_h
#define sevenSegment4d12p_h

#include "Arduino.h"

class sevenSegment4d12p
{
public:
    sevenSegment4d12p(int latchPin, int clockPin, int dataPin, int digit1, int digit2, int digit3, int digit4, boolean comAnode = false); // Constructor
    void prepareNumber(int number, boolean withDot = false);
    void prepareNoNumber(void);
    void allDeactivate(void);
    void allActivate(void);
    void activateDigit(int digit);
    void deactivateDigit(int digit);
private:
    int _digit1;
    int _digit2;
    int _digit3;
    int _digit4;
    int _colon;
    int _latchPin;
    int _clockPin;
    int _dataPin;
    int _comAnode;
    
    byte _binDigits[10] =  //seven segment digits in bits (LSBFIRST) Common Cathode
    {
        B11111100, // 0
        B01100000, // 1
        B11011010, // 2
        B11110010, // 3
        B01100110, // 4
        B10110110, // 5
        B10111110, // 6
        B11100000, // 7
        B11111110, // 8
        B11100110, // 9
    };
    byte _binDigitsInverse[10] =  //seven segment digits in bits (LSBFIRST) Common Anode
    {
        B00000011, // 0
        B10011111, // 1
        B00100101, // 2
        B00001101, // 3
        B10011001, // 4
        B01001001, // 5
        B01000001, // 6
        B00011111, // 7
        B00000001, // 8
        B00011001, // 9
    };
};
#endif
