#include <sevenSegment.h>

sevenSegment counter(8, 12, 11, 2, 3, 4, 5); // latchPin,clockPin,dataPin,digit1,digit2,digit3,digit4

unsigned long milisegundos;
int segundosTotales = 0;
int minutos;
int segundos;

int d1;
int d2;
int d3;
int d4;

int analogPinButton = 0;
//int timeButton = 10;
int leftButton = 9;
//int centerButton = 7;
int rightButton = 6;

boolean punto;

unsigned long comienzo;
unsigned long acelerador;


void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    
    //comienzo=0;
    //acelerador = 0;
    
    counter.FourDigitsAllDeactivate();
    
    //pinMode(timeButton, INPUT);
    pinMode(leftButton, INPUT);
    //pinMode(centerButton, INPUT);
    pinMode(rightButton, INPUT);
    
    
    
    
}

void loop() {
    
    Serial.println(analogRead(analogPinButton));
    
  // put your main code here, to run repeatedly:

//    if (digitalRead(timeButton) == HIGH){
//        comienzo=millis();
//        segundosTotales=0;
//        Serial.println("time");
//    }
//    if (digitalRead(leftButton) == HIGH){
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
//    if (digitalRead(rightButton) == HIGH){
//        comienzo=millis();
//        segundosTotales=0;
//        Serial.println("right");
//    }
    
    
    
    
    
    if (false) {
    milisegundos = millis();
    milisegundos += acelerador;
    if ((milisegundos-comienzo)/1000 > segundosTotales) segundosTotales++;
    
    minutos = segundosTotales/60;
    segundos = segundosTotales%60;
    
    
    d1=minutos/10;
    d2=minutos%10;
    
    d3=segundos/10;
    d4=segundos%10;
    
    punto = false;
    if (milisegundos%1000 < 500) punto = true;
    
    
    counter.prepareNumberSR(d1);
    counter.FourDigitsActivateDigit(1);
    counter.FourDigitsDeactivateDigit(1);

    counter.prepareNumberSR(d2,punto);
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
