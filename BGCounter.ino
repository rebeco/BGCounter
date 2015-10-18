#include <sevenSegment4d12p.h>


sevenSegment4d12p counter4d(8, 9, 10, 4, 5, 6, 7, true); // latch,clock,data,digit1,digit2,digit3,digit4,inverse


// -- Time counter and 7segment display --//

unsigned long milliseconds;
unsigned long totalMillis;
unsigned long millisPerPlayer[5] = {0, 0, 0, 0, 0};
unsigned long newStartTotal;
unsigned long newStartPerPlayer[5] = {0, 0, 0, 0, 0};

int d1;
int d2;
int d3;
int d4;

bool digitPoint;
bool colon=false;

unsigned long forceTimeStart; // debug

// -------------------- //

// -- Rotary Encoder -- //

int encoderPinA = 2; // Tienen que coincidir con los que usamos para las interrupciones
int encoderPinB = 3; // Tienen que coincidir con los que usamos para las interrupciones

volatile int lastEncoded = 0;

volatile long encoderValue = 0;
volatile long lastEncoderValue;

int encoderBrake = 3;

int lastMSB = 0;
int lastLSB = 0;

int valueRotary;
int lastValueRotary;

int encoderPinButton = 11;

// -------------------- //


// -- Generals -- //

int buzzerPin = 12;

int settingStep = 1;
int counterMode = 1;
int running = 0;
int lastMode = 4; // it has 1,2,3,4 modes

bool pause = true;
int pauseDisplay = 0;
unsigned long pauseTime;

// --------------- //


// -- Buttons and leds -- //

int analogPinButtons = A0;
int valueAnalogPinButtons;

int pinLed1 = A1;
int pinLed2 = A2;
int pinLed3 = A3;
int pinLed4 = A4;
int pinLed5 = A5;

int playersPinLed[5] = {pinLed1, pinLed2, pinLed3, pinLed4, pinLed5};

int readButton = 0;
unsigned long timeLastReadButton = 0;
int breakReadButton = 200;

unsigned long timeLastReadEncoderButton = 0;
int breakReadEncoderButton = 200;

// ------------- //


// -- Players -- //

bool players[5] = {false, false, false, false, false};
int activePlayer;

// ------------- //




void setup() {

    Serial.begin(9600);
    
    pinMode(encoderPinButton, INPUT);
    
    pinMode(pinLed1, OUTPUT);
    pinMode(pinLed2, OUTPUT);
    pinMode(pinLed3, OUTPUT);
    pinMode(pinLed4, OUTPUT);
    pinMode(pinLed5, OUTPUT);
    
    pinMode(buzzerPin, OUTPUT);
    
    pinMode(encoderPinA, INPUT);
    pinMode(encoderPinB, INPUT);
    
    digitalWrite(encoderPinA, HIGH); //Pullup resistor ON
    digitalWrite(encoderPinB, HIGH); //Pullup resistor ON
    
    //Call updateEncoder() function when change high/low
    //on Interrupt 0 (pin 2) or Interrupt 1 (pin 3)
    attachInterrupt(0, updateEncoder, CHANGE);
    attachInterrupt(1, updateEncoder, CHANGE);
    
    // Display all components on init
    buzzer();
    allPlayerLeds(HIGH);
    for (int n=0; n<1000; n++) drawClock(0,true);
    allPlayerLeds(LOW);
    buzzer();

}

void loop() {
    
    milliseconds = millis(); // Take the milliseconds from arduino init
 
    if (settingStep == 1) {
        // We are setting the mode
        
        // Blink actual mode
        if (milliseconds%1000 > 500) drawDigit(4,counterMode);
        
        // Calculate new setting mode
        valueRotary = encoderValue/encoderBrake;
        
        if (valueRotary > lastMode) {
            valueRotary = lastValueRotary;
            encoderValue = lastEncoderValue;
        }
        else if (valueRotary < 1) {
            valueRotary = 1;
            encoderValue = encoderBrake;
        }
        
        lastValueRotary = valueRotary;
        lastEncoderValue = encoderValue;
        
        counterMode = valueRotary;
        
        if (readEncoderButton()) {
            
            for (int n=0; n<4000; n++) drawDigit(4,counterMode);
            settingStep = 2;
        }
    }
    
    if (settingStep == 2) {
        // We are setting color players
        
        if (counterMode == 1 || counterMode == 2) {
            
            if (milliseconds%1000 > 500) allPlayerLeds(HIGH);
            else for (int n=0; n<5 ; n++) if (!players[n]) playerLed(n+1, LOW);
          
            readButton = readAnalogButtons();
            
            if (players[readButton-1] == false) players[readButton-1] = true;
            else players[readButton-1] = false;
            
            if (readEncoderButton()) {
                bool thereArePlayers = false;
                
                for (int n=0; n<5; n++) if (players[n]) thereArePlayers = true;
            
                if (thereArePlayers) {
                    settingStep = 0; // Mode 1 and 2 don't need more settings.
                    
                    for (int n=0; n<5; n++) if (players[n]) millisPerPlayer[n] = 0; // Time ascending
    
                    totalMillis = 0;
                    running = 1;
                    
                    // Reset encoder
                    valueRotary = 0;
                    lastValueRotary = 0;
                    encoderValue = 0;
                    
                    newStartTotal = milliseconds; // Set the init time
                }
            }
        }
    }
    
    if (settingStep == 3) {
        // We are setting init time
        
        
    }
    
 
    if (running && (counterMode == 1 || counterMode == 2)) {
        // Executing time ascending
        
        if (counterMode == 2) totalMillis = milliseconds - newStartTotal; // Always count the total play time in counter mode 2
        
        if (pause) {
            
            // Rotate over total and player times. pauseDisplay=0 shows total time, other shows player time
            valueRotary = encoderValue/encoderBrake;
            
            if (valueRotary > lastValueRotary) {
                int n = pauseDisplay+1;
                if (n == 6) pauseDisplay = 0;
                else {
                    while (true) {
                        if (players[n-1]) {
                            pauseDisplay=n;
                            break;
                        }
                        else if (n == 5) {
                            pauseDisplay = 0;
                            break;
                        }
                        n++;
                    }
                }
            }
            else if (valueRotary < lastValueRotary) {
                int n = pauseDisplay-1;
                if (n == 0) pauseDisplay = 0;
                else {
                    if (n == -1) n = 5;
                    while (true) {
                        if (n == 0) {
                            pauseDisplay = 0;
                            break;
                        }
                        else if (players[n-1]) {
                            pauseDisplay=n;
                            break;
                        }
                        n--;
                    }
                }
            }
            
            lastValueRotary = valueRotary;
            lastEncoderValue = encoderValue;
            
            if (pauseDisplay == 0) {
                activatePlayersLeds(); // Display all players
                pauseTime = totalMillis;
            }
            else {
                allPlayerLeds(LOW);
                playerLed(pauseDisplay, HIGH); // Display only active player
                pauseTime = millisPerPlayer[pauseDisplay-1];
            }
            
            
            if (milliseconds%1000 > 500) drawClock(pauseTime, true); // Blink total play time or player time
            
            
            // Pause all counters
            newStartTotal = milliseconds - totalMillis;
            for (int n=0; n<5; n++) if (players[n]) newStartPerPlayer[n] = milliseconds - millisPerPlayer[n];
        
            readButton = readAnalogButtons();
            if (readButton != 0 && players[readButton-1]) {
                activePlayer = readButton;
                pauseDisplay = 0; // Reset pauseDisplay for the next pause
                pause = false;
            }
            
        }
        else {
            // Active player counts
            activatePlayersLeds();
            
            if (counterMode == 1) totalMillis = milliseconds - newStartTotal; // Only count the total play time if it's not in pause mode and in counter mode 1
            millisPerPlayer[activePlayer-1] = milliseconds - newStartPerPlayer[activePlayer-1];
            
            colon = true;
            if (milliseconds%1000 > 500) colon = false;
            
            if (milliseconds%1000 < 500) playerLed(activePlayer, LOW); // Blink active player
            
            drawClock(millisPerPlayer[activePlayer-1], colon);
            
            for (int n=0; n<5; n++) { // Pause for all players except active player
                if (n != activePlayer-1) newStartPerPlayer[n] = milliseconds - millisPerPlayer[n];
            }
            
            readButton = readAnalogButtons();
            if (readButton != 0 && players[readButton-1]) {  // Change active player
                activePlayer = readButton;
            }
            if (readEncoderButton()) pause = true; // Pause
        }
    }
}



void drawDigit(int digit, int value) {
    counter4d.prepareNumber(value);
    counter4d.activateDigit(digit);
    counter4d.deactivateDigit(digit);
}

void drawClock (unsigned long timeToDraw, bool colon) {
    int seconds, minutes, hours, toSeconds;
    bool pointHour;
    
    toSeconds = timeToDraw/1000;
    
    pointHour = true;
    if (toSeconds < 3600) {
        
        minutes = toSeconds/60;
        seconds = toSeconds%60;
        
        d1=minutes/10;
        d2=minutes%10;
        
        d3=seconds/10;
        d4=seconds%10;
        
        pointHour = false;
    }
    else {
        hours = toSeconds/3600;
        minutes = (toSeconds%3600)/60;
        
        d1=hours/10;
        d2=hours%10;
        
        d3=minutes/10;
        d4=minutes%10;
    }
    
    counter4d.prepareNumber(d1);
    counter4d.activateDigit(1);
    counter4d.deactivateDigit(1);
    
    counter4d.prepareNumber(d2, colon);
    counter4d.activateDigit(2);
    counter4d.deactivateDigit(2);
    
    counter4d.prepareNumber(d3);
    counter4d.activateDigit(3);
    counter4d.deactivateDigit(3);
    
    counter4d.prepareNumber(d4, pointHour);
    counter4d.activateDigit(4);
    counter4d.deactivateDigit(4);
}

void allPlayerLeds (int value) {
    for (int n=0; n<5 ; n++) digitalWrite(playersPinLed[n], value);
}

void activatePlayersLeds () {
    for (int n=0; n<5 ; n++) if (players[n]) digitalWrite(playersPinLed[n], HIGH); else digitalWrite(playersPinLed[n], LOW);
}

void playerLed (int player, int value) {
    for (int n=0; n<5 ; n++) if (player == n+1) digitalWrite(playersPinLed[n], value);
}

int readEncoderButton() {
    bool returnValue = false;
    
    if (milliseconds > timeLastReadEncoderButton + breakReadEncoderButton) {
        timeLastReadEncoderButton = milliseconds;
        if (digitalRead(encoderPinButton)==HIGH) returnValue = true;
    }
    if (returnValue) buzzer();
    return returnValue;
}

int readAnalogButtons() {
    int returnValue;
    valueAnalogPinButtons = analogRead(analogPinButtons);
    
    if (milliseconds > timeLastReadButton + breakReadButton) {
        timeLastReadButton = milliseconds;
        if (valueAnalogPinButtons > 1000) returnValue = 1;
        else if (valueAnalogPinButtons > 900 && valueAnalogPinButtons < 1000) returnValue = 2;
        else if (valueAnalogPinButtons > 800 && valueAnalogPinButtons < 900) returnValue = 3;
        else if (valueAnalogPinButtons > 600 && valueAnalogPinButtons < 800) returnValue = 4;
        else if (valueAnalogPinButtons > 450 && valueAnalogPinButtons < 600) returnValue = 5;
        else returnValue = 0;
    }
    else returnValue = 0;
    
    if (returnValue != 0) buzzer();
        
    return returnValue;
}

void buzzer() {
    digitalWrite(buzzerPin, HIGH);
    delay(5);
    digitalWrite(buzzerPin, LOW);
}


void updateEncoder()
{
    int MSB = digitalRead(encoderPinA); //MSB = Most signicant bit
    int LSB = digitalRead(encoderPinB); //LSB = Least significant bit
    
    int encoded = (MSB << 1) |LSB;
    int sum  = (lastEncoded << 2) | encoded;
    
    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;
    
    
    lastEncoded = encoded;
}

