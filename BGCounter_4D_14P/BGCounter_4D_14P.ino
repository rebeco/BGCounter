#include <sevenSegment4d14p.h>

sevenSegment4d14p counter(8, 7, 6, 2, 3, 4, 5, 9); // latchPin,clockPin,dataPin,digit1,digit2,digit3,digit4,colon





void setup() {
    Serial.begin(9600);
    
    counter.allDeactivate();
    
 }


void loop() {
  

    
    
    char strClock[] = "56 79";
    
    
    if (millis()%1000 > 500) strClock[2]=':';
    else strClock[2]=' ';
    
    drawClock(strClock);

   
    
}


void drawClock(char* joker) {
    Serial.println(joker[2]);
    char buffer;
    buffer = joker[0];
    counter.prepareNumber(atoi(&buffer));
    counter.activateDigit(1);
    counter.deactivateDigit(1);
    buffer = joker[1];
    counter.prepareNumber(atoi(&buffer));
    counter.activateDigit(2);
    counter.deactivateDigit(2);
    buffer = joker[3];
    counter.prepareNumber(atoi(&buffer));
    counter.activateDigit(3);
    counter.deactivateDigit(3);
    buffer = joker[4];
    counter.prepareNumber(atoi(&buffer));
    counter.activateDigit(4);
    counter.deactivateDigit(4);
    if (joker[2] == ':') {
        counter.activateColon();
        counter.deactivateColon();
    }
}