#include <sevenSegment4d14p.h>
#include <sevenSegment1d10p.h>

sevenSegment4d14p counter4d(A0, A1, A2, 5, 6, 7, 8, A3); // latch,clock,data,digit1,digit2,digit3,digit4,colon
sevenSegment1d10p counter1d(0,1,4); // latch,clock, data


// -- Contador de tiempo --//

unsigned long milisegundos;
int segundosTotales = 0;
int minutos;
int segundos;

int d1;
int d2;
int d3;
int d4;

unsigned long comienzo;
unsigned long acelerador;

// -------------------- //



// -- Rotary Encoder -- //

int encoderPinA = 2; // Tienen que coincidir con los que usamos para las interrupciones
int encoderPinB = 3; // Tienen que coincidir con los que usamos para las interrupciones
//int encoderSwitchPin = 4; //Pulsador

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

long lastencoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;

int valueRotary;

int encoderPinButton = A4;


// -------------------- //


// -- Generales -- //

int modo;
bool digitPoint;

int playPinButton = A5;

// --------------- //



void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);
    
    comienzo=0;
    acelerador = 0;
        counter4d.allDeactivate();
 
    pinMode(encoderPinButton, INPUT);
    pinMode(playPinButton, INPUT);
    
    pinMode(encoderPinA, INPUT);
    pinMode(encoderPinB, INPUT);
    digitalWrite(encoderPinA, HIGH); //Pullup resistor ON
    digitalWrite(encoderPinB, HIGH); //Pullup resistor ON
    //Llama a la funcion updateEncoder() cuando hay un cambio de estado high/low
    //en Interrupt 0 (pin 2) o en Interrupt 1 (pin 3)
    attachInterrupt(0, updateEncoder, CHANGE);
    attachInterrupt(1, updateEncoder, CHANGE);
    
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
    
    
    counter4d.prepareNumber(d1);
    counter4d.activateDigit(1);
    counter4d.deactivateDigit(1);
    
    counter4d.prepareNumber(d2);
    counter4d.activateDigit(2);
    counter4d.deactivateDigit(2);
    
    counter4d.prepareNumber(d3);
    counter4d.activateDigit(3);
    counter4d.deactivateDigit(3);
    
    counter4d.prepareNumber(d4);
    counter4d.activateDigit(4);
    counter4d.deactivateDigit(4);

    if (milisegundos%1000 < 500) {
        counter4d.activateColon();
        counter4d.deactivateColon();
    }
    
    
    
    // Rotary Encoder
    
    valueRotary = encoderValue/4;
    
    digitPoint = false;
    if(digitalRead(encoderPinButton)==HIGH || digitalRead(playPinButton)==HIGH) digitPoint = true;
    
    counter1d.drawNumber(valueRotary, digitPoint);
    
    
}


void updateEncoder()
{
    int MSB = digitalRead(encoderPinA); //MSB = bit mas significativo
    int LSB = digitalRead(encoderPinB); //LSB = bit menos significativo
    
    int encoded = (MSB << 1) |LSB; //convierte el pin 2 en un solo valor
    int sum  = (lastEncoded << 2) | encoded; //añadiendolo al valor anterior
    
    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;
    
    lastEncoded = encoded; //guarda el valor para la proxima vez
}

