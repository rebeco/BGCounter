#include <sevenSegment.h>

sevenSegment counter(7, 8, 9, 10, 11, 12, 13, 6, 2, 3, 4, 5); // a,b,c,d,e,f,g,dot,digit1,digit2,digit3,digit4

unsigned long milisegundos;
int segundosTotales = 0;
int minutos;
int segundos;

int d1;
int d2;
int d3;
int d4;

boolean punto;

unsigned long comienzo;
unsigned long acelerador;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
    
    comienzo=0;
    acelerador = 0;
        counter.FourDigitsAllDeactivate();
    
}

void loop() {
  // put your main code here, to run repeatedly:

    
    
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
    
    
    counter.prepareNumber(d1);
    counter.FourDigitsActivateDigit(1);
    counter.FourDigitsDeactivateDigit(1);
    
    counter.prepareNumber(d2,punto);
    counter.FourDigitsActivateDigit(2);
    counter.FourDigitsDeactivateDigit(2);
    
    counter.prepareNumber(d3);
    counter.FourDigitsActivateDigit(3);
    counter.FourDigitsDeactivateDigit(3);
    
    counter.prepareNumber(d4);
    counter.FourDigitsActivateDigit(4);
    counter.FourDigitsDeactivateDigit(4);

    
    


}
