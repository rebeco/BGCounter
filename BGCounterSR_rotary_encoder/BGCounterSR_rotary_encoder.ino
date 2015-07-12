#include <sevenSegment.h>

sevenSegment counter(8, 12, 11, 2, 3, 4, 5); // latchPin,clockPin,dataPin,digit1,digit2,digit3,digit4

//unsigned long milisegundos;
//int segundosTotales = 0;
int configMinutes;
int minutes;
int seconds;
int hours;
unsigned long totalTime; // In milliseconds
//int segundos;
//

unsigned long startMillis = 0;
unsigned long actualMillis = 0;

int d1;
int d2;
int d3;
int d4;

int analogPinButton = 0;
int pinLeftButton = 9;
int pinRightButton = 6;

boolean point;

//unsigned long comienzo;
//unsigned long acelerador;

int LEFTBUTTON = 1;
int RIGHTBUTTON = 2;
int SELECTBUTTON = 3;
int TIMEBUTTON = 4;

int lastMode = 3; // it has 1,2,3 modes

int counterMode = 1;
int settingStep = 1;

int longWaitAfterPress = 200;
int shortWaitAfterPress = 50;



boolean stopCounter = true;


void setup() {
    Serial.begin(9600);
    
    //comienzo=0;
    //acelerador = 0;
    
    counter.FourDigitsAllDeactivate();

    pinMode(pinLeftButton, INPUT);
    pinMode(pinRightButton, INPUT);
    
    
    // Blink all segments on init
    for (int n=0; n<1500; n++) {
        if (n%500 < 250) counter.prepareNumberSR(8,true);
        else counter.prepareNoNumberSR();
        counter.FourDigitsActivateDigit(1);
        counter.FourDigitsDeactivateDigit(1);
        
        if (n%500 < 250) counter.prepareNumberSR(8,true);
        else counter.prepareNoNumberSR();
        counter.FourDigitsActivateDigit(2);
        counter.FourDigitsDeactivateDigit(2);
        
        if (n%500 < 250) counter.prepareNumberSR(8,true);
        else counter.prepareNoNumberSR();
        counter.FourDigitsActivateDigit(3);
        counter.FourDigitsDeactivateDigit(3);
        
        if (n%500 < 250) counter.prepareNumberSR(8,true);
        else counter.prepareNoNumberSR();
        counter.FourDigitsActivateDigit(4);
        counter.FourDigitsDeactivateDigit(4);
        }
    
}

int readButton () {
    int result;
    if (digitalRead(pinLeftButton) == HIGH) result = LEFTBUTTON;
    if (digitalRead(pinRightButton) == HIGH) result = RIGHTBUTTON;
    
    return result;
}

int readAnalogButton () {
    int result;
    int read;
    
    read = analogRead(analogPinButton);
    if (read > 170 and read < 190) result = SELECTBUTTON;
    if (read < 30 and read > 5) result = TIMEBUTTON;
    
    return result;
}

void loop() {
    
    //Serial.println(analogRead(analogPinButton));
    
    
    if (settingStep == 1) {
        // We are configuring the mode
        
        // blink the actual mode
        if (millis() % 1000 < 500) counter.prepareNumberSR(counterMode);
        else counter.prepareNoNumberSR();
        counter.FourDigitsActivateDigit(1);
        counter.FourDigitsDeactivateDigit(1);
        
        counter.prepareNoNumberSR();
        counter.FourDigitsActivateDigit(2);
        counter.FourDigitsDeactivateDigit(2);
        counter.FourDigitsActivateDigit(3);
        counter.FourDigitsDeactivateDigit(3);
        counter.FourDigitsActivateDigit(4);
        counter.FourDigitsDeactivateDigit(4);
        
        // are we changing the mode?
        if (readButton() == LEFTBUTTON) {
            delay(longWaitAfterPress);
            if (counterMode == 1) counterMode = lastMode;
            else counterMode--;
        }
        if (readButton() == RIGHTBUTTON) {
            delay(longWaitAfterPress);
            if (counterMode == lastMode) counterMode = 1;
            else counterMode++;
        }
        
        // are we selecting the mode?
        if (readAnalogButton() == SELECTBUTTON) {
            settingStep = 2;
            if (counterMode == 1) {
                // Init de mode 1
                totalTime = 0;
            }
        }
    }
    
    if (settingStep == 2) {
        // We are configuring the selected mode
        
        // Fix the selected mode
        counter.prepareNumberSR(counterMode);
        counter.FourDigitsActivateDigit(1);
        counter.FourDigitsDeactivateDigit(1);
        
        if (counterMode == 1) {
            // We are setting the mode 1. Ascending counter.
            settingStep = 3; // No need special config.
        }
        
        if (counterMode == 2) {
            // We are setting the mode 2. Descending counter.
            minutes = totalTime / 60;
            
            d2 = 0;
            d3 = 0;
            d4 = 0;
            
            if (configMinutes < 10) d4 = configMinutes;
            
            if (configMinutes > 10 and configMinutes < 100) {
                d3 = configMinutes / 10;
                d4 = configMinutes % 10;
            }
            
            if (configMinutes > 100) {
                d2 = configMinutes / 100;
                d3 = (configMinutes % 100) / 10;
                d4 = (configMinutes % 100) % 10;
            }
            
            // Blink minutes
            if (millis() % 400 < 200) counter.prepareNumberSR(d2);
            else counter.prepareNoNumberSR();
            counter.FourDigitsActivateDigit(2);
            counter.FourDigitsDeactivateDigit(2);
            if (millis() % 400 < 200) counter.prepareNumberSR(d3);
            else counter.prepareNoNumberSR();
            counter.FourDigitsActivateDigit(3);
            counter.FourDigitsDeactivateDigit(3);
            if (millis() % 400 < 200) counter.prepareNumberSR(d4);
            else counter.prepareNoNumberSR();
            counter.FourDigitsActivateDigit(4);
            counter.FourDigitsDeactivateDigit(4);
            
            // are we changing the minutes?
            if (readButton() == LEFTBUTTON) {
                delay(shortWaitAfterPress);
                if (configMinutes != 0) totalTime -= 60;
            }
            if (readButton() == RIGHTBUTTON) {
                delay(shortWaitAfterPress);
                if (configMinutes != 999) totalTime +=60;
            }
            
        }
        
    }
    
    if (settingStep == 3) {
        // It is counting
        if (counterMode == 1) {
            // Executing mode 1. Ascending counter.
            
            if (readAnalogButton() == TIMEBUTTON) {
                delay(longWaitAfterPress);
                if (stopCounter) stopCounter = false;
                else stopCounter = true;
            }
            if (! stopCounter) {
                // Its running
                actualMillis = millis();
                totalTime = actualMillis - startMillis;
                
                seconds = (actualMillis-startMillis)/1000;
                minutes = seconds/60;
                hours = minutes/60;
            }
            else {
                // Its stopped
                startMillis = millis() - totalTime;
            }
            
            if (minutes < 60) {
                // config for minutes:seconds
                d1=minutes/10;
                d2=minutes%10;
                d3=(seconds%60)/10;
                d4=(seconds%60)%10;
            }
            else {
                // config for hours:minutes
                d1=hours/10;
                d2=hours%10;
                d3=(minutes%60)/10;
                d4=(minutes%60)%10;
            }
            
            point = false;
            if ((actualMillis-startMillis)%1000 < 500 or stopCounter) point = true;
            
            counter.prepareNumberSR(d1);
            counter.FourDigitsActivateDigit(1);
            counter.FourDigitsDeactivateDigit(1);
            
            counter.prepareNumberSR(d2,point);
            counter.FourDigitsActivateDigit(2);
            counter.FourDigitsDeactivateDigit(2);
            
            counter.prepareNumberSR(d3);
            counter.FourDigitsActivateDigit(3);
            counter.FourDigitsDeactivateDigit(3);
            
            counter.prepareNumberSR(d4);
            counter.FourDigitsActivateDigit(4);
            counter.FourDigitsDeactivateDigit(4);
        }
        
        
        
    }
    
    








  // put your main code here, to run repeatedly:

//    if (digitalRead(timeButton) == HIGH){
//        comienzo=millis();
//        segundosTotales=0;
//        Serial.println("time");
//    }
//    if (digitalRead(pinLeftButton) == HIGH){
//        comienzo=millis();
//        segundosTotales=0;
//        Serial.println("left");
//    }
//    if (digitalRead(centerButton) == HIGH){
//        comienzo=millis();
//        segundosTotales=0;
//        Serial.println("center");
//    }
//    
//    if (digitalRead(pinRightButton) == HIGH){
//        comienzo=millis();
//        segundosTotales=0;
//        Serial.println("right");
//    }
    
    
//    if (false) {
//        milisegundos = millis();
//        milisegundos += acelerador;
//        if ((milisegundos-comienzo)/1000 > segundosTotales) segundosTotales++;
//        
//        minutos = segundosTotales/60;
//        segundos = segundosTotales%60;
//        
//        
//        d1=minutos/10;
//        d2=minutos%10;
//        
//        d3=segundos/10;
//        d4=segundos%10;
//        
//        punto = false;
//        if (milisegundos%1000 < 500) punto = true;
//        
//        
//        counter.prepareNumberSR(d1);
//        counter.FourDigitsActivateDigit(1);
//        counter.FourDigitsDeactivateDigit(1);
//        
//        counter.prepareNumberSR(d2,punto);
//        counter.FourDigitsActivateDigit(2);
//        counter.FourDigitsDeactivateDigit(2);
//        
//        counter.prepareNumberSR(d3);
//        counter.FourDigitsActivateDigit(3);
//        counter.FourDigitsDeactivateDigit(3);
//        
//        counter.prepareNumberSR(d4);
//        counter.FourDigitsActivateDigit(4);
//        counter.FourDigitsDeactivateDigit(4);
//        
//    }
    
}
