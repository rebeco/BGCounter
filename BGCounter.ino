#include <sevenSegment4d12p.h>


sevenSegment4d12p counter4d(8, 9, 10, 4, 5, 6, 7, true); // latch,clock,data,digit1,digit2,digit3,digit4,inverse


// -- Contador de tiempo --//

unsigned long milisegundos;
unsigned long milisTotales;
unsigned long milisPerPlayer[5] = {0, 0, 0, 0, 0};
unsigned long newStartTotal;
unsigned long newStartPerPlayer[5] = {0, 0, 0, 0, 0};


int d1;
int d2;
int d3;
int d4;

unsigned long comienzo; // Para hacer que el reloj empiece a partir de unos milisegundos (debug)

// -------------------- //



// -- Rotary Encoder -- //

int encoderPinA = 2; // Tienen que coincidir con los que usamos para las interrupciones
int encoderPinB = 3; // Tienen que coincidir con los que usamos para las interrupciones
//int encoderSwitchPin = 4; //Pulsador

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


// -- Generales -- //

bool digitPoint;
bool colon=false;

int buzzerPin = 12;

int settingStep = 1;
int counterMode = 1;
int running = 0;
int lastMode = 3; // it has 1,2,3 modes

bool pause = true;
int pauseDisplay = 0;
unsigned long pauseTime;

// --------------- //


// -- Buttons -- //

int analogPinButtons = A0;
int valueAnalogPinButtons;

int pinLed1 = A1;
int pinLed2 = A2;
int pinLed3 = A3;
int pinLed4 = A4;
int pinLed5 = A5;

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

    Serial.begin(9600); // Si lo descomentamos inutiliza los pines digitales 0 y 1.
    
    
    
    counter4d.allDeactivate();
 
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
    
  //Llama a la funcion updateEncoder() cuando hay un cambio de estado high/low
    //en Interrupt 0 (pin 2) o en Interrupt 1 (pin 3)
    attachInterrupt(0, updateEncoder, CHANGE);
    attachInterrupt(1, updateEncoder, CHANGE);
    
    
    // Blink all segments on init
//    for (int n=0; n<1500; n++) {
//        counter4d.allDeactivate();
//        if (n%500 < 250) {
//         counter4d.prepareNumber(8);
//            digitalWrite(pinLed1, HIGH);
//            digitalWrite(pinLed2, HIGH);
//            digitalWrite(pinLed3, HIGH);
//            digitalWrite(pinLed4, HIGH);
//            digitalWrite(pinLed5, HIGH);
//            digitalWrite(buzzerPin, HIGH);
//        }
//        else {
//            counter4d.prepareNoNumber();
//            digitalWrite(pinLed1, LOW);
//            digitalWrite(pinLed2, LOW);
//            digitalWrite(pinLed3, LOW);
//            digitalWrite(pinLed4, LOW);
//            digitalWrite(pinLed5, LOW);
//            digitalWrite(buzzerPin, LOW);
//        }
//        counter4d.activateDigit(1);
//        counter4d.deactivateDigit(1);
//        counter4d.activateDigit(2);
//        counter4d.deactivateDigit(2);
//        counter4d.activateDigit(3);
//        counter4d.deactivateDigit(3);
//        counter4d.activateDigit(4);
//        counter4d.deactivateDigit(4);
//    }

}

void loop() {
    
    milisegundos = millis();
 
    if (settingStep == 1) {
        // We are configuring the mode
        
        // Blink actual mode
        if (milisegundos%1000 > 500) counter4d.prepareNumber(counterMode);
        else counter4d.prepareNoNumber();
        
        counter4d.activateDigit(4);
        counter4d.deactivateDigit(4);
        
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
            
            for (int n=0; n<3000; n++) {
                if (n%400 > 200) counter4d.prepareNumber(counterMode);
                else counter4d.prepareNoNumber();
                counter4d.activateDigit(4);
                counter4d.deactivateDigit(4);
            }
            counter4d.prepareNumber(counterMode);
            
            counter4d.activateDigit(4);
            delay(1000);
            counter4d.deactivateDigit(4);
            
            settingStep = 2;
        }
    }
    
    if (settingStep == 2) {
        // We are setting players
        
        if (counterMode == 1) {
            
            if (milisegundos%1000 > 500) allPlayerLedsOn();
            else {
                if (!players[0]) digitalWrite(pinLed1, LOW);
                if (!players[1]) digitalWrite(pinLed2, LOW);
                if (!players[2]) digitalWrite(pinLed3, LOW);
                if (!players[3]) digitalWrite(pinLed4, LOW);
                if (!players[4]) digitalWrite(pinLed5, LOW);
            }
          
            
            readButton = readAnalogButtons();
            
            if (players[readButton-1] == false) players[readButton-1] = true;
            else players[readButton-1] = false;
            
            if (readEncoderButton()) {
                bool thereArePlayers = false;
                
                for (int n=0; n<5; n++) {
                    if (players[n]) thereArePlayers = true;
                }
                
                if (thereArePlayers) {
                    settingStep = 0; // Mode 1 dont need more setting.
                    
                    comienzo=3595000; // Time ascending. Change from 0 to debug
                    comienzo=0;
                    
                    for (int n=0; n<5; n++) {
                        if (players[n]) milisPerPlayer[n] = comienzo;
                    }
                    milisTotales = 0;
                    running = 1;
                    
                    valueRotary = 0;
                    lastValueRotary = 0;
                    encoderValue = 0;
                }
            }
        }
    }
    
    if (settingStep == 3) {
        // We are setting init time
        
        
    }
    
 
    if (running && counterMode == 1) {
        // Executing time ascending
        
        if (pause) {
            
            // Rotate over total and player times
            valueRotary = encoderValue/encoderBrake;
            
            Serial.print(lastValueRotary);
            Serial.print(" - ");
            Serial.println(valueRotary);
            if (valueRotary > lastValueRotary) {
                int n = pauseDisplay+1;
                if (n == 6) {
                    
                    pauseDisplay = 0;
                }
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
            
            if (pauseDisplay == 0) activatePlayerLeds();
            else {
                allPlayerLedsOff();
                activatePlayerLed(pauseDisplay);
            }
            
            
            if (pauseDisplay == 0) pauseTime = milisTotales;
            else pauseTime = milisPerPlayer[pauseDisplay-1];
           
            if (milisegundos%1000 > 500) drawClock(pauseTime, true); // Blink total play time
            
            
            // Pause all counters
            newStartTotal = milisegundos - milisTotales;
            for (int n=0; n<5; n++) if (players[n]) newStartPerPlayer[n] = milisegundos - milisPerPlayer[n];
        
            readButton = readAnalogButtons();
            if (readButton != 0 && players[readButton-1]) {
                activePlayer = readButton;
                pauseDisplay = 0;
                pause = false;
            }
            
        }
        else { // Active player counts
            activatePlayerLeds();
            
            milisTotales = milisegundos - newStartTotal;
            milisPerPlayer[activePlayer-1] = milisegundos - newStartPerPlayer[activePlayer-1];
            
            if (milisegundos%1000 > 500) colon = false;
            else colon = true;
            
            if (milisegundos%1000 < 500) deactivatePlayerLed(activePlayer);
            
            drawClock(milisPerPlayer[activePlayer-1], colon);
            
            for (int n=0; n<5; n++) { // Pause for all player except active player
                if (n != activePlayer-1) newStartPerPlayer[n] = milisegundos - milisPerPlayer[n];
            }
            
            readButton = readAnalogButtons();
            if (readButton != 0 && players[readButton-1]) {  // Change active player
                activePlayer = readButton;
            }
            if (readEncoderButton()) pause = true; // Pause
            
            
            
        }
        
    }
    
    
    
    
//    milisegundos = millis();
//    
//    milisegundos += comienzo;
//    
//    if (milisegundos/1000 > segundosTotales) segundosTotales++;
//    
//        
//    minutos = segundosTotales/60;
//    segundos = segundosTotales%60;
//    
// 
//    d1=minutos/10;
//    d2=minutos%10;
//    
//    d3=segundos/10;
//    d4=segundos%10;
//    
//    if (milisegundos%1000 > 500) colon = false;
//    else colon = true;
//    
//    
//    counter4d.prepareNumber(d1);
//    counter4d.activateDigit(1);
//    counter4d.deactivateDigit(1);
//    
//    counter4d.prepareNumber(d2, colon);
//    counter4d.activateDigit(2);
//    counter4d.deactivateDigit(2);
//    
//    counter4d.prepareNumber(d3);
//    counter4d.activateDigit(3);
//    counter4d.deactivateDigit(3);
//    
//    counter4d.prepareNumber(d4);
//    counter4d.activateDigit(4);
//    counter4d.deactivateDigit(4);

    
        
}

void drawClock (unsigned long timeToDraw, bool colon) {
    int seconds, minutes, hours, toSeconds;
    bool pointHour;
    
    toSeconds = timeToDraw/1000;
    
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
        
        pointHour = true;
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

void allPlayerLedsOn () {
    digitalWrite(pinLed1, HIGH);
    digitalWrite(pinLed2, HIGH);
    digitalWrite(pinLed3, HIGH);
    digitalWrite(pinLed4, HIGH);
    digitalWrite(pinLed5, HIGH);
}

void allPlayerLedsOff () {
    digitalWrite(pinLed1, LOW);
    digitalWrite(pinLed2, LOW);
    digitalWrite(pinLed3, LOW);
    digitalWrite(pinLed4, LOW);
    digitalWrite(pinLed5, LOW);
}

void activatePlayerLeds () {
    
    if (players[0]) digitalWrite(pinLed1, HIGH); else digitalWrite(pinLed1, LOW);
    if (players[1]) digitalWrite(pinLed2, HIGH); else digitalWrite(pinLed2, LOW);
    if (players[2]) digitalWrite(pinLed3, HIGH); else digitalWrite(pinLed3, LOW);
    if (players[3]) digitalWrite(pinLed4, HIGH); else digitalWrite(pinLed4, LOW);
    if (players[4]) digitalWrite(pinLed5, HIGH); else digitalWrite(pinLed5, LOW);
    
}

void activatePlayerLed (int player) {
    if (player == 1) digitalWrite(pinLed1, HIGH);
    if (player == 2) digitalWrite(pinLed2, HIGH);
    if (player == 3) digitalWrite(pinLed3, HIGH);
    if (player == 4) digitalWrite(pinLed4, HIGH);
    if (player == 5) digitalWrite(pinLed5, HIGH);
}

void deactivatePlayerLed (int player) {
    if (player == 1) digitalWrite(pinLed1, LOW);
    if (player == 2) digitalWrite(pinLed2, LOW);
    if (player == 3) digitalWrite(pinLed3, LOW);
    if (player == 4) digitalWrite(pinLed4, LOW);
    if (player == 5) digitalWrite(pinLed5, LOW);
}




int readEncoderButton() {
    bool returnValue = false;
    
    if (milisegundos > timeLastReadEncoderButton + breakReadEncoderButton) {
        timeLastReadEncoderButton = milisegundos;
        if (digitalRead(encoderPinButton)==HIGH) returnValue = true;
    }
    return returnValue;
}

int readAnalogButtons() {
    int returnValue;
    valueAnalogPinButtons = analogRead(analogPinButtons);
    
    if (milisegundos > timeLastReadButton + breakReadButton) {
        timeLastReadButton = milisegundos;
        if (valueAnalogPinButtons > 1000) returnValue = 1;
        else if (valueAnalogPinButtons > 900 && valueAnalogPinButtons < 1000) returnValue = 2;
        else if (valueAnalogPinButtons > 800 && valueAnalogPinButtons < 900) returnValue = 3;
        else if (valueAnalogPinButtons > 600 && valueAnalogPinButtons < 800) returnValue = 4;
        else if (valueAnalogPinButtons > 450 && valueAnalogPinButtons < 600) returnValue = 5;
        else returnValue = 0;
    }
    else returnValue = 0;
    
    return returnValue;
}

void buzzer() {
    digitalWrite(buzzerPin, HIGH);
    delay(50);
    digitalWrite(buzzerPin, LOW);
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

