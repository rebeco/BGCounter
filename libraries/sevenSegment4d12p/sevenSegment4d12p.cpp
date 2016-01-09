/*
 sevenSegment4d12p.h - Library for control a senven segment digit display 4 digits and 14 pin.
 Created by Pedro León, May 14, 2015.
 Released into the public domain.
 */


#include "Arduino.h"
#include "sevenSegment4d12p.h"

#define DIGIT_OFF LOW
#define DIGIT_ON HIGH
#define SEGMENT_OFF LOW
#define SEGMENT_ON HIGH

#define BRIGHT_DIGIT 100

sevenSegment4d12p::sevenSegment4d12p(int latchPin, int clockPin, int dataPin, int digit1, int digit2, int digit3, int digit4, boolean comAnode)
{
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    pinMode(digit1, OUTPUT);
    pinMode(digit2, OUTPUT);
    pinMode(digit3, OUTPUT);
    pinMode(digit4, OUTPUT);
    _latchPin = latchPin;
    _clockPin = clockPin;
    _dataPin = dataPin;
    _digit1 = digit1;
    _digit2 = digit2;
    _digit3 = digit3;	
    _digit4 = digit4;
    _comAnode = comAnode;
}

void sevenSegment4d12p::allDeactivate(void)
{
    digitalWrite(_digit1, DIGIT_OFF);
    digitalWrite(_digit2, DIGIT_OFF);
    digitalWrite(_digit3, DIGIT_OFF);
    digitalWrite(_digit4, DIGIT_OFF);
}

void sevenSegment4d12p::allActivate(void)
{
    digitalWrite(_digit1, DIGIT_ON);
    digitalWrite(_digit2, DIGIT_ON);
    digitalWrite(_digit3, DIGIT_ON);
    digitalWrite(_digit4, DIGIT_ON);
}

void sevenSegment4d12p::activateDigit(int digit)
{
    switch (digit) {
        case 1:
            digitalWrite(_digit1, DIGIT_ON);
            break;
        case 2:
            digitalWrite(_digit2, DIGIT_ON);
            break;
        case 3:
            digitalWrite(_digit3, DIGIT_ON);
            break;
        case 4:
            digitalWrite(_digit4, DIGIT_ON);
            break;
    }
    delayMicroseconds(BRIGHT_DIGIT);
}

void sevenSegment4d12p::deactivateDigit(int digit)
{
    switch (digit) {
        case 1:
            digitalWrite(_digit1, DIGIT_OFF);
            break;
        case 2:
            digitalWrite(_digit2, DIGIT_OFF);
            break;
        case 3:
            digitalWrite(_digit3, DIGIT_OFF);
            break;
        case 4:
            digitalWrite(_digit4, DIGIT_OFF);
            break;
    }
}

void sevenSegment4d12p::prepareNoNumber(void)
{
    digitalWrite(_latchPin, SEGMENT_OFF);
    if (_comAnode) shiftOut(_dataPin, _clockPin, LSBFIRST, B11111111);
    else shiftOut(_dataPin, _clockPin, LSBFIRST, B00000000);

    digitalWrite(_latchPin, SEGMENT_ON);
}

void sevenSegment4d12p::prepareNumber(int number, boolean withDot)
{
    // I assume this map
    // Q0 = a
    // Q1 = b
    // Q2 = c
    // Q3 = d
    // Q4 = e
    // Q5 = f
    // Q6 = g
    // Q7 = dot
    
    byte binDigit;
    
    if (_comAnode) binDigit = _binDigitsInverse[number];
    else binDigit = _binDigits[number];
    
    if (withDot) {
        if (_comAnode) binDigit -= B00000001;
        else binDigit += B00000001;
    }

    digitalWrite(_latchPin, SEGMENT_OFF);
    shiftOut(_dataPin, _clockPin, LSBFIRST, binDigit);
    digitalWrite(_latchPin, SEGMENT_ON);
}

